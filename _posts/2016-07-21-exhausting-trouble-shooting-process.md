---
layout: post
comments: true
section-type: post
title: 很费人的问题排查记录
category: tech
tags: [ 'tutorial' ]
---
### ###引言

    用来记录很难得问题、简单却难搞定的问题、很赞的问题的解决过程。

## ###ssh 登录ECS报错

​	使用ssh登录时，可使用Public Key Authentication(PKA),以避免反复输入密码和信息泄露(如脚本中)。
	类似github，gitlab等的实现，需要本地存储一个Private Key,登录时用于和服务端存储的Public Key做鉴权操作。
	使用阿里ECS设置ssh PKA的时候，除过生成本地的private key/public key两个文件外，可使用ssh-copy-id将public key上传到ECS上的~/.ssh/authorized_keys文件。修改ECS的/etc/ssh/ssh_config文件，打开其中的RSAAuthentication,PubkeyAuthentication,AuthorizedKeysFile(%h/.ssh/authorized_keys存储健全公钥文件存储路径，%h代表当前需要鉴权的用户目录，%u代表用户)。设置后需要重启ssh服务。
	按理，设置好ssh，设置好公钥私钥就可以正常免密登陆了，可惜一直提示Permission denied，翻来覆去搞了半天依然没有搞明白为什么，最后才发现和若干目录的权限设置有关系。

[关于git的ssh permission denied原因汇总 - 飞奔的猪](http://zqscm.qiniucdn.com/data/20130903005919/index.html)
	参考上述文章，设置好权限，OK。
​	