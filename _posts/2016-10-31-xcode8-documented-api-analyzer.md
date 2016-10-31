---
layout: post
comments: true
section-type: post
title: Xcode8 API文档解析
category: tech
tags: [ 'tutorial' ]
---
## 背景

       iOS API的start/deprecated的判断一个重要基础是获取到每个API的具体版本支持信息。
       在Xcode8之前，
    /Applications/Xcode7.app/Contents/Developer/Documentation/DocSets/com.apple.adc.documentation.iOS.docset/Contents/Resources/Tokens/Objective-C
    包含了所有的API参考文档，解析这些xml即可获取到C:API,OC:Class/Protocol的Property/Method的对应信息。
       Xcode8出现之后，此处空空如也，该怎么办呢?

![Xcode7 API文档](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/xcode7-docset-apiinfo-preview.png)

## Xcode8 API文档去哪了？

	思路:
	Xcode8从源代码跳转Documentation正常，即使断网也OK，说明Xcode8的文档也在本地。
	首先找到Xcode的PID
	ps -A 
	笔者这里是23592
	打印出所有此pid打开的文件
	lsof -p 23592

![xcode8-opened-files](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/xcode8-opened-files.png)
​	
	从图中可以发现疑似文档文件的map.db和cache.db


## db分析

​        通过对map.db和cache.db的分析，不难发现，map中存储的是token/request_key这类数据，再结合request_key隐射到cache.db中的blob中去。结合response表中的uncompressed_size字段(3876)，对比blob导出的内容大小(1833)，可以发现blob应该就是我们的目标数据，且被压缩过了。

![xcode8-document-map-db-preview](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/xcode8-document-map-db-preview.png)

![xcode8-document-cache-db-preview](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/xcode8-document-cache-db-preview.png)

![xcode8-document-api-compressed-initwithframe-ref](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/xcode8-document-api-compressed-initwithframe-ref.png)

## blob内容的解压缩

	打开多个blob内容不难发现他们有同样的模式
		bvx......bvx$
	Google大法搜索下:
	
![google-bgxd-compress-preview](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/google-bgxd-compress-preview.png)
	
	可以发现git@github.com:lzfse/lzfse.git已经提供了一种对于此类数据的解压缩方法，调用
	lzfse -decode -i ~/Desktop/initwithframe.txt -o initwithframe_out.txt
	即可得到我们需要的数据:

![xcode8-document-api-uncompressed-initwithframe-ref](https://github.com/kangwang1988/kangwang1988.github.io/raw/master/img/xcode8-document-api-uncompressed-initwithframe-ref.png)

## 从blob中获取API支持版本信息
	blob解压缩之后的内容是json格式的，关于版本的信息存储在如下的字段里(以-[UIViewController viewDidUnload]为例)
	"y": [
	{
		"p": "ios",->平台
		"de": "6.0",->deprecated系统号
		"ir": "3.0",->起始支持系统号
		"cr": "10.0"->当前支持系统号
	}

## 整合
	
	基于以上的分析，采用以下的步骤获取所有API的版本信息:
	1.基于sqlite提供的C API，遍历所有的map记录，并通过request_key获取到所有的token对应的blob字节流
	2.使用lzfse提供的库，将blob字节流解压缩获得json文件
	3.分析所有的json文件，并根据
		"l": "occ"->Objective-C
		"k": "cl"->类
			 "intf"->协议
			 "intfp"->协议属性
			 "instm"->实例方法
			 "instp"->实例属性
			 "clm"->类方法
			 "cldata"->类属性
			 "intfdata"->协议属性
			 "intfm"->协议方法
			 "intfcm"->协议方法
			 "func"->C APi
			 "struct"->结构体
			 "union"->联合
			 "structp"->结构体属性
			 "unionp"->联合属性
			 "data"/"enum"/"tdef"/"tag"/"econst"->其他常亮
			 "macro"->宏
		获取各种类型包含的tokens的对应版本信息。

## 问题
	
		虽然使用上述的方法已经可以获得所有的token的版本信息，但是实际中也发现了一个问题，即部分类和协议属性的版本描述不准确，典型地如:
		+[NSNotificationCenter defaultCenter]

![NSNotificationCenter_defaultCenter_DB](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/NSNotificationCenter_defaultCenter_DB.png)

![NSNotificationCenter_defaultCenter_Doc](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/NSNotificationCenter_defaultCenter_Doc.png)

![NSNotificationCenter_defaultCenter_Web](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/NSNotificationCenter_defaultCenter_Web.png)

![NSNotificationCenter_defaultCenter_Xcode](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/NSNotificationCenter_defaultCenter_Xcode.png)

对于这种典型的问题，需要在发现后自己再修改下生成的API->版本信息数据Json,才能正常使用。

[Contact me](mailto:kang.wang1988@gmail.com)