---
layout: post
comments: true
section-type: post
title: 如何从苹果AppStore下载旧版本的APP(使用iTunes+Charles)
category: tech
tags: [ 'tutorial' ]
---

### 准备工作
	Mac+iTunes+Charles(v3.11.4)
	Charles需要安装Charles Root Certificate.
	1.Charles下载证书*.pem
		Help->SSL Proxying->Save Charles Root Certificates...
	2.钥匙串安装证书
		File->Import Items
		
### 步骤(以微信为例)

	1.打开itunes和Charles，在iTunes中搜索微信，并点击Get按钮。
	
![itunes-wechat](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/itunes-wechat.png)

	2.在Charles的请求列表中寻找形如https://*-buy.itunes.com(buyProduct.woa)等的请求，右键请求Enable SSL Proxying并打开Breakpoints.
	3.从Charles捕获的2中buyProduct.woa的返回中查找softwareVersionExternalIdentifiers字段及其对应的列表Value.此列表代表的版本号从最早到最新排列。
	
![charles-itunes-buy](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/charles-itunes-buy.png)

	4.在iTunes中删除先前下载好的包，并再次执行1，将会发现Charles断点执行到。编辑其中的appExtVrsId为3中的老版本的vrsId并点击Execute，可能需要在断点再次执行到时点击Execute。
	
![charles-itunes-breakpoint](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/charles-itunes-breakpoint.png)

	5.在iTunes已安装的App中可以查找到需要的老版本的APP.
	
![itunes-myapp-wechat](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/itunes-myapp-wechat.png)