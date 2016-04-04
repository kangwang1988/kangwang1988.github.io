---
layout: post
section-type: post
title: 使用CocoaPods管理Xcode项目依赖
category: tech
tags: [ 'technique' ]
--- 

![Cocoa Pods](http://nairteashop.org/wp-content/uploads/2013/11/CocoaPods.png)

## CocoaPods是什么

	**CocoaPods** 是用于Swift和Objective-C等Cocoa项目的依赖管理器.目前已经支持超过*10,000*个库,可以帮助你优雅地管理项目.

### 使用

#### 安装 

	CocoaPods使用Ruby构建，使用OS X上默认的Ruby即可安装。这也是推荐的操作.

	$ sudo gem install cocoapods

#### 检查更新

	$ sudo gem update cocoapods --verbose
	verbose开启，可以帮助诊断更新详情。

#### 源管理

	$ gem sources --remove https://rubygems.org
	$ gem sources -a http://ruby.taobao.org
	$ gem sources -l

	由于ruby默认软件源使用的Amazon云服务被墙，可通过上述命令将官方ruby源替换(或者vpn)。

#### 使用CocoaPods

	安装好CocoaPods后，执行pod setup，可将资源库上的podspec文件拉到本地库，默认位于~/.cocoapods/repos/master下面。

#### Podfile
 	Podfile是一个用来描述一个或多个Xcode项目中的Target质检的依赖关系.Podfile经常用于创建一个称之为default的隐式target指向用户工程中的第一个target,文件一般命名位Podfile。


	一个略微复杂的Podfile例如:

	source 'https://github.com/CocoaPods/Specs.git' # 指定Source
	source 'https://github.com/Artsy/Specs.git'
	platform :ios, '8.0' #
	inhibit_all_warnings!
	xcodeproj 'MyProject'
	pod 'ObjectiveSugar', '~> 0.5'
	pod 'Artsy+UILabels', '~> 1.0'
	target :test do
		pod 'OCMock', '~> 2.0.1'
	end
	post_install do |installer|
		installer.project.targets.each do |target|
			puts target.name
		end
	end

具体参见[Podfile.html](https://guides.cocoapods.org/syntax/podfile.html)

#### 使用CocoaPods

	1. 创建一个Podfile(位于项目根目录)，添加依赖。
	2. 运行 $ pod install 在你的工程路径下。
	3. 打开XXX.xcworkspace构建。
	4. 如果需要同已有的workspace集成，在podfile中添加 workspace 'XXXWorkspace'即可.
#### Podfile.lock

	首次运行pod install之后生成了Podfile.lock文件,此文件跟踪了每个已安装的Pod.Podfile.lock的使用使得即使一个新的Pod version可用，CocoaPods依旧会尊重Podfile.lock中描述的Pod版本，除非Podfile中的依赖关系被更新或者调用了pod update.也就避免了由于依赖关系上发生不可预料的问题引发的头痛问题。

####  Version notes

	1. '>0.1' 版本号高于0.1
	2. '>= 0.1' 版本号0.1及以上版本
	3. '<0.1' 版本号低于0.1
	4. '<=0.1' 版本号0.1及以下版本
	5. '~> 0.1.2' 版本号0.1.2和0.2之间

#### pod notes

	pod 'PodName', :path => 'localpath' #use localpath 
	pod 'PodName', :git => 'gitpath' #use master branch
	pod 'PodName', :git => 'gitpath', :branch => 'dev' #use dev branch
	pod 'PodName', :git => 'gitpath', :tag => '0.8.0' #use tag
	pod 'PodName', :git => 'gitpath', :commit => '082f8319af' #use commit

#### 参考资料

	[CocoaPods](https://cocoapods.org)