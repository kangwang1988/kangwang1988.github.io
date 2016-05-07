---
layout: post
comments: true
section-type: post
title: mac启动自签名的基于IP的https服务
category: tech
tags: [ 'tutorial' ]
---

### 背景
	
	iOS APP开发过程中，常常需要版本的分发。对此苹果提供了itms-service协议可用。一个典型的itms-service形如:itms-services://?action=download-manifest&url=http://xxx.xxx.com/xxx.plist。plist文件指定了真正需要下载的包的相关信息，如版本，显示名，图标，下载链接等。ios7.1以后，iOS要求plist scheme必须为https。而在实际工作中，常常因为具体情况限制，我们常采用ip地址部署。此文即讨论了如何使用自签名的基于ip的ssl证书来启动https服务。
	
### 环境
	MAMP
	
### 构造ssl证书	a.构造https的自签名证书		openssl genrsa 1024 >server.key (生成私钥)		openssl req -new -key server.key > server.csr(生成证书请求)		openssl req -x509 -days 3650 -key server.key -in server.csr >server.crt(生成证书)	PS.生成证书时，请确保证书的CommonName与服务器IP地址一致()每次服务器地址发生变化时都应重新生成证书，此证书默认拷贝一份放到htdoc的根目录下供用户下载)	b.修改Apache对应的httpd.conf,确保httpd-ssl.conf和mod_ssl.so被包含进来。	c.修改Apache对应的httpd-ssl.conf，指定服务器证书和key文件路径		SSLCertificateFile "/Applications/MAMP/conf/apache/server.crt"		SSLCertificateKeyFile "/Applications/MAMP/conf/apache/server.key"
		###	其他	
	重启MAMP，一切正常的话https://192.168.1.1(请替换为时机的ip地址)可以正常访问。	如果在iOS设备中访问的话，需要安装server.crt证书。可在http的index页面中提供下载链接，在Safari中点击并下载安装一次(安装后不再需要)，以后就可以在手机中正常使用了。