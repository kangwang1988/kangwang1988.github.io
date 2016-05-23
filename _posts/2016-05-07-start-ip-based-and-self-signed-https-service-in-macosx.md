---
layout: post
comments: true
section-type: post
title: mac下启动自签名且基于IP的https服务
category: tech
tags: [ 'tutorial' ]
---

### 背景
	
	iOS APP开发过程中，常常需要版本的分发。对此苹果提供了itms-service协议可用。一个典型的itms-service形如:itms-services://?action=download-manifest&url=http://yourhost/xxx.plist。plist文件指定了真正需要下载的包的相关信息，如版本，显示名，图标，下载链接等。ios7.1以后，iOS要求plist scheme必须为https。而在实际工作中，常常因为具体情况限制，我们常采用ip地址部署。此文即讨论了如何使用自签名的基于ip的ssl证书来启动https服务。
	
### 环境
	MAMP
	
### 构造ssl证书

