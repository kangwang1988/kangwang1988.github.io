---
layout: post
comments: true
section-type: post
title: 如何搭建iOS中的DailyBuild
category: tech
tags: [ 'tutorial' ]
---

### 原理
	shell获取仓库代码，打包ipa，上传ipa(如果需要)
	xcodebuild生成.app文件和签名
	itms-service协议安装包

[run_release.sh](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/others/run_release.sh)
		
### 使用到的工具
	xcode
	git
	bash
	iPhoneConfigurationUtility管理provision
	apache服务器搭建https服务器
	
### 代码管理工具(以git为例)
	
	git clone repository-url -b branchname path-to-your-working-copy
	此处建议使用ssh(git@git.xxx.com:xxx/xxx.git)登陆。
	1.使用ssh-keygen生成相应的公钥和私钥(如github_rsa,github_rsa.pub)，并在配置文件~/.ssh/config中写入字段如下:
		Host github.com
		HostName github.com
		PreferredAuthentications publickey
		IdentityFile ~/.ssh/github_rsa
		User yourusername
	当有多个公钥私钥对存在时，除过生成公钥私钥置于~/.ssh目录下外，还需要添加类似上述配置。
	2.将公钥添加到git仓库中的sshkey中以便后续使用。
	
[git_common.sh](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/others/git_common.sh)
###	编译和签名
	xcodebuild -workspace "$workspaceName" -scheme "$schemeName" -sdk "$sdkName" -configuration "$configuration" GCC_PREPROCESSOR_DEFINITIONS="$gccMacros" PROVISIONING_PROFILE="$provisionUUID" CONFIGURATION_BUILD_DIR="$tmpSysRoot" BUILT_PRODUCTS_DIR="$tmpSysRoot"
	编译分发时，sdkName选择iphoneos,configuration如Release或Debug，自定义宏请写入gccMacros,需要用到的provision建议提前安装到系统并用iPhone Configuration Utility查看无误，将其Profile Identifier写入provisionUUID。
	查看可用的编译设置使用xcodebuild -showBuildSettings
	查看可用的scheme使用xcodebuild -list
	
[compile_common.sh](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/others/compile_common.sh)
### 打包
	编译成功后生成的.app文件拷贝到一个Payload文件夹中后，压缩并将其重命名为xxx.ipa即可得到需要的app包。
	
[package_common.sh](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/others/package_common.sh)
### OTA安装
	ipa包生成后，可利用网页嵌入itms-service连接来下载:
	itms-services://?action=download-manifest&url=https://url-to-plist.plist
	plist文件包含了需要下载的ipa的相关信息和实际的ipa链接地址。如果遇到没法将plist文件部署到https连接的问题，可使用基于ip的自签名ssl证书。
	
[ip-based self-signed ssl certificate](http://kangwang1988.github.io/tech/2016/05/07/start-ip-based-and-self-signed-https-service-in-macosx.html)
### 代码/资源检查等插件
	可在run_release.sh中某个时间点添加代码/资源检查等功能的shell调用。
### 错误排查
	本地代码编译通过后，CI编译打包错误往往是环境和证书等的问题，具体请查看相应shell日志。
	将真机连接到Mac上，使用Xcode中的Devices查看安装进度和错误信息可以及时了解安装过程中的各种信息，如果有问题，基本上是签名等的问题。需要排查CI上是否已经安装了正确且匹配的证书和provision并保证run_release.sh中的调用正确。
	
	