# XcodeGhost 剖析


## 引子
	最近XcodeGhost引起的风波愈演愈烈，众多知名APP中枪，各种说法也有。作为一个iOS Developer，觉得有必要基于自己的专业知识，给大家提供一个新的认知维度，如有纰漏或是错误，还请不吝赐教。
## 何为XcodeGhost
	说白了，XcodeGhost就是一段恶意代码(不管作者主观意愿如何)，作者通过篡改苹果官方的Xcode安装包，在其中加入了某原本不存在的CoreService库文件，使用此Xcode版本编译出的APP由于加载了此库，导致其中恶意代码被执行.
	说到这里，开发者为什么要去下载这些个有问题的版本呢？说到这里，不得不吐槽下国内的网络了，由于连接到mas(Mac App Store)的连接速度不给力，而一个安装包就好几G，大家也都习惯使用Google(爬墙是开发的必备技能)去搜索相关的安装包dmg并使用迅雷下载了。这里又分两种情况，使用迅雷下载来自苹果的安装包或使用百度云。恶意代码的提供者通过在各大论坛留言，引诱大家去下载被注入问题库的安装包。
	未经证实的消息称有人通过迅雷下载的来自苹果的安装包也被注入问题库，个人猜测可能是迅雷在一开始判定时，在并不知晓dmg散列值的情况下就映射到了有问题的dmg包。不然若P2P开始下载，必然要求目标文件和下载文件必须散列值一致才能正常完成下载和组合。
	这也要求小伙伴们养成校验文件SHA/MD5等hash值的好习惯，下载结束后求散列值并与官网提供的值作比较。
	Mac下可使用
		md5 file 
		shasum file
	求文件file的md5/sha值。
## CoreService文件分析
	本来想下载一个CoreService dump一下看看有哪些API调用的，没找到。就使用下已有的分析吧:
	
	(截图来自http://drops.wooyun.org/news/8864)
## XcodeGhost源文件解析及其危害
## 如何检测你的APP是否中招
### 设置代理，检测异常流量
### 分析ipa文件
### Fetch apis

	class-dump-z ./CLPDemo.app/CLPDemo > CLPDemo.api

## Get the private api list.
>	Private apis are api which developers are prevented from using. In order to get private apis list for checking, We have to get all apis provided by apple first(api-a). By excluding those public apis declared in the header files, documented apis from .docset, we will get a list of private apis.

	1.Get apis lists(api-a) from a framework.(e.g Foundation.framework)
	class-dump --arch i386 /path2IosSdks/SDKs/iPhoneSimulator.sdk/System/Library/Frameworks/Foundation.framework/Foundation > Foundations.api
	2.Get apis from XXXFramework/*.h(api-b).
	2.Get documented apids(api-c)
	sqlite3 /path2docset/com.apple.adc.documentation.iOS.docset/Contents/Resources/docSet.dsidx "select ZDECLARATION from ZTOKENMETAINFORMATION" >~/documented.api
	ps. Some steps may need further processing before well organized.
	
>   There is also some public-repository where all apis are presented.
	Refer to: https://github.com/nst/iOS-Runtime-Headers.git.
	
## Useful commands

	otool:提取并显示ios下目标文件的相关信息，包括头部，加载命令，各个段，共享库，动态库等等。它拥有大量的命令选项，是一个功能强大的分析工具，当然还可以做反汇编的工具使用。
	lipo:是一款创建或处理通用文件的命令行工具.
	nm:列出一个函数库文件中的符号表。
	class-dump:dump出破解之后的iOS二进制文件的头文件信息。
	sqlite3:处理sqlite数据库的命令行工具。

## 更多
Contact [KyleWong](mailto:kang.wang1988@gmail.com) for more.