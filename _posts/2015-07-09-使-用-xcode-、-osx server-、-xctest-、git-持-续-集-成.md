---
layout: post
section-type: post
title: 使用Xcode、OSX Server、 XCTest、Git持续集成
category: tech
tags: [ 'technique' ]
---

## 何为持续集成

	在软件工程实践中，通过把所有人的工作副本集合起来进行一次或多次构建的过程成为**持续集成(Continuous integration)**。持续集成被作为极限编程的一部分引入实践，其主要是为了防止在早期极限编程中提到的集成的问题.早期是用来在测试驱动开发的实践中用于自动化测试的，通过在开发者本地机器中运行所有的单元测试，验证其均通过测试以提交到主干，这样就避免了一个开发者进行中的工作打扰到另一个开发者。后来CI这一概念引入了构建服务器，其通过周期性地运行测试用例或者在提交后讲结果报告给开发者。除过单元测试外，使用持续集成的组织通常使用构建服务器来实现质量控制-每次一点，频繁使用.除过单元和集成测试外，还包括额外的静态和动态测试,衡量和分析性能，从源代码中抽取和形成文档，以及帮助手动QA过程。持续的质量控制旨在改善软件质量，减少分发时间，用于替换原有的开发结束后的质量控制，这也通传统的频繁集成以简化集成理念一致。
	同样地，持续集成拓展出的持续发布也确保了主干上的软件随时处于克发布的状态，使得实际部署的过程很快。

### iOS中的持续集成
![Xcode continous integration workflow](https://developer.apple.com/library/ios/documentation/IDEs/Conceptual/xcode_guide-continuous_integration/art/continuous_integration_2x.png)

	此框架中，OSX Server(原本是一套独立的系统，从Lion开始作为一个工具包存在)充当服务器角色，从托管库中获取代码并完成分析、构建、测试、打包等持续集成过程，支持主流的Git/SVN代码管理。开发者通过在Developer Machine中完成代码和测试用例编写后，提交给托管库，通过设置Bot,可在每次提交后，由服务器自动运行持续集成的过程，并将构建结果通知用户。

### 环境搭建

	1.下载并安装OSX Server

[点击前往Mac Store下载](https://itunes.apple.com/us/app/os-x-server/id883878097?mt=12)
$19.9刀感觉贵的话，可以去百度云盘搜索。

	OSX Server下载完成后，打开并安装，选中你需要管理的机器并登陆。
	在左侧服务中选择Xcode服务并打开使能，右侧控制面板中选中用于持续集成的Xcode.app，也可以填写开发着团队,添加设备等。
	Xcode中的储存库此处可不填写，在Xcode中创建Repository的时候选择在服务器创建即可。

	2.Xcode、XCTest和Bot
	* Xcode中创建工程，最后一步Source Control选择Create Git repository on <your osx server hostname/ip address>.
	* 如果没有在创建工程的时候指定，也可以通过Source Control -> Working Copy ->Configure->Remote中的添加按钮去添加repository.
	* 开发项目，使用XCTest编写测试用例。
	* Project->Create Bot，根据Wizard的提示，逐步完成Bot设置。需要注意的是，Server应选中OS X Server所对应的Server.如果最开始选中了Integrate immediately将在Bot创建完成后开始集成。Test with选中设备时，默认的All iOS Devices and Simulators会使得系统针对所有已经安装的Simulators去构建。当设备上安装了过多Simulators的时候，这一步骤将很慢，因此选中那些自己需要构建的平台即可。After Intergration的时候，可以选择Notify Committers以接收集成结果的通知邮件。

	3.运行持续集成

	* 如果创建Bot的时候选中了On Commit，那么提交代码即会运行Bot，否则将按照创建时的设置区运行Bot。
	* Bot的运行主要包括了:Preparing-Checkout-Build-Test-Archive-处理结果-Upload等。
	* 一切正常的话，在Server中打开设置运行的Bot，可看到如下的结果:
![Bot Run Result](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/CIBotResult.png)

	4.注意事项

	* 集成过程中，可能因为设置的Bot支持的Simulators过多造成等待时间过久的问题，应通过修改Bot支持的平台为实际需要。
	* 集成过程中，可能因为Provision/Certificate不正确造成错误。需要明确的是，即使OSX Server同Developer Machine为同一台机器，其使用的证书和Provision也不是同一路径。Server使用的证书位于System Keychain，Developer的位于Login，Server使用的Provision位于/Library目录而不是Developer的~/Library目录。
	* 集成过程中，可能因为最终CodeSign阻塞在后面,修改OSXServer对应的System Keychain下面的证书Access Control使之始终容许codesign访问即可。
	* 发送邮件的问题，需要配置Postfix使之支持内网和外网邮件发送。具体的配置参见:

[Enabling postfix for outbound relay via Gmail on OS X Lion (and newer OSX versions)](http://blog.anupamsg.me/2012/02/14/enabling-postfix-for-outbound-relay-via-gmail-on-os-x-lion-11/)
