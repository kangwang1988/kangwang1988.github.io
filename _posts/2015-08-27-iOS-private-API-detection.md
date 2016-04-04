---
layout: post
section-type: post
title: iOS private API detection
category: tech
tags: [ 'technique']
---


## Analyze your app(CLPDemo.app)
### Fetch apis

	class-dump-z ./CLPDemo.app/CLPDemo > CLPDemo.api

## Get the private api list.

	Private apis are api which developers are prevented from using. In order to get private apis list for checking, We have to get all apis provided by apple first(api-a). By excluding those public apis declared in the header files, documented apis from .docset, we will get a list of private apis.

	1.Get apis lists(api-a) from a framework.(e.g Foundation.framework)
	class-dump --arch i386 /path2IosSdks/SDKs/iPhoneSimulator.sdk/System/Library/Frameworks/Foundation.framework/Foundation > Foundations.api
	2.Get apis from XXXFramework/*.h(api-b).
	2.Get documented apids(api-c)
	sqlite3 /path2docset/com.apple.adc.documentation.iOS.docset/Contents/Resources/docSet.dsidx "select ZDECLARATION from ZTOKENMETAINFORMATION" >~/documented.api
	ps. Some steps may need further processing before well organized.
	
	There is also some public-repository where all apis are presented.
	Refer to: https://github.com/nst/iOS-Runtime-Headers.git.
	
## Useful commands

	otool:提取并显示ios下目标文件的相关信息，包括头部，加载命令，各个段，共享库，动态库等等。它拥有大量的命令选项，是一个功能强大的分析工具，当然还可以做反汇编的工具使用。
	lipo:是一款创建或处理通用文件的命令行工具.
	nm:列出一个函数库文件中的符号表。
	class-dump:dump出破解之后的iOS二进制文件的头文件信息。
	sqlite3:处理sqlite数据库的命令行工具。