# XcodeGhost 剖析

![Xcode](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/_images/xcodeghost_4.jpg)
![Ghost](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/_images/xcodeghost_5.jpeg)
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
	本来想下载一个CoreService dump一下看看有哪些API调用的，没找到。就使用下已有的分析吧。
![Pic 1](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/_images/xcodeghost_1.jpg)
![Pic 2](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/_images/xcodeghost_2.png)

	咋一看这个APP也就搜集了些APP的信息和设备信息，并传到了某网站，危害并没有那么大，看官别急，一会我们再分析下源码。
	(截图来自http://drops.wooyun.org/news/8864)
## XcodeGhost源文件解析
	时间越闹越大，有个人出来说是他干的，并且放出了相关的源代码(可信度较高)，本文的重点也在于对于其源码的分析。
[查看源码](https://github.com/XcodeGhostSource/XcodeGhost)
	
	恶意代码包括两部分:
	UIWindow(didFinishLaunchingWithOptions)和UIDevice(AppleIncReservedDevice)
	作者偷偷地加载了有问题的Library到包中，并巧妙利用了苹果的runtime机制，使用Category污染你的APP。通过重写原有的makeKeyAndVisible(开发者在APP启动的时候通常会调用此函数)函数，其添加了一个定时任务和四个通知处理函数。
	1.定时任务:每15s执行一次，其作用在于当APP在前台运行时，获取你的隐私信息(具体看后文)，将其DES加密(密钥为@"stringWithFormat",其可在服务端对其对称解密，对于捕捉到异常流量而又不知道加密算法的人只是一堆乱码),然后将数据通过http请求发送到其设定的服务器(http://init.icloud-analysis.com,说对方关闭服务器就没事的亲，DNS劫持听过没？路由器中的DNS记录被篡改分分钟搭建起此服务器，btw 来路不明的Wifi不要随便连啊).
	2.监听APP进入Active状态(到前台)的通知，以@“launch”为参数(此参数将同个人隐私数据一同打包)，发送网络请求给上述后台服务器。
	3.监听APP退出Active状态(APP切换)的通知，以@“resignActive”为参数发送网络请求。
	4.监听APP将要Terminate(杀掉)的通知，以@"terminate"为参数发送网络请求。
	5.监听APP进入后台(退到后台)的通知，以@"suspend"为参数发送网络请求。
	看起来也没什么是吧，too young, too simple.
	当后台接收到网络请求之后，会发送Response给客户端(具体内容天知道)，客户端对其进行处理，首先解密，如果让休息，那就乖乖滴停一会(免得被发现啊...),检查如果服务端返回了某些有效信息，则分别进行处理。
	1.弹框:如果服务端返回了可供显示的Alert内容，你的App就莫名地谈一个框，如果点击了确定，给后端发一个@"alertView"为参数的请求，并且应用内弹出Appstore.如下所示:
	
![应用内AppStore](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/_images/xcodeghost_3.jpg)
	
	容我脑补一下，这就如果你在同花顺里弹了一个框让你下载一个金融理财的软件，你以为是同花顺推的，下载下来，一用duang，账户空空如也了。
	
	2.URLScheme:如果服务端返回了configUrl,客户端会使用scheme打开APP(可能因为后台配置而有延时).
	科普一下:苹果支持以下的NSURL构建以调用不同的系统组件:
	a.@"tel://8004664411" 打电话
	b.@"sms://466453 发短信
	c.@"mailto://devprograms@apple.com" 发邮件
	d.@"http://www.makebetterthings.com/blogs/" 打开浏览器
	e.@"http://maps.google.com/maps?q=%@" 打开地图
	
	王炸来了:
	苹果支持APP通过scheme调用打开其他的APP!!!
	QQ的url是 mqq:// 
	微信是weixin:// 
	淘宝taobao:// 
	点评dianping:// dianping://search 
	微博 sinaweibo:// 
	名片全能王camcard:// 
	weico微博weico:// 
	支付宝alipay:// 
	豆瓣fm：doubanradio:// 
	微盘 sinavdisk:// 
	网易公开课ntesopen://
	美团 imeituan:// 
	京冬openapp.jdmoble:// 
	人人renren:// 
	我查查 wcc:// 
	1号店wccbyihaodian:// 
	有道词典yddictproapp:// 
	知乎zhihu://
	优酷 youku://
	而且这些scheme均可带参数调用，假设sinaweibo://支持发微博，呵呵，瞬间就可以调用数以千万级的肉鸡去发微博了，那画面太美我不敢看...
	又或者微信里打开Safari跳到某个伪装成微信页面的连接，盗你的微信密码分分钟...
	
	3.直接应用内出AppStore，同1，只是更流氓而已。

## 危害

>人的一半是天使，一半是魔鬼。

	尽管作者说他没做什么坏事，只是为了一些看似无害的事情，我还是有必要邪恶一下，告诉大家这事情的危害有多大。
	微信里面跳转到一个让你输入微信用户名密码的貌似微信的页面，用户很有可能以为是微信的连接，中了...
	微信里面弹出一个框，给你推一款APP，可能是比较好的，也可能纯粹就是窃取你隐私的，你以为是腾讯出品，中了...
	这个作者哪天做了一个APP，想推广一下，给微信APP(可以通过用户上传的bundleid区分)用户发送其下载地址，我想，其APPStore排名绝对蹭蹭蹭涨上去。
	作者哪天想赚点广告费，给微信APP发一个URL连接，大家都点过去，依照点击量转费用的作者瞬间不菲收入；
	作者既可以说发一些广告给你，也可以推病毒给你，完全在于他怎么搞，更要命的是，如果用户不能更新至安全的APP或者将不安全的APP卸载，某些有能力的坏人可以篡改DNS做一个和作者设定的服务器域名一致的服务器，呵呵，这已经超出原始作者的控制范围了...
	btw 我不是故意要黑微信，微信6.2.5版本确认中招的，而且用微信更能说明问题。
	
## 如何检测你的APP是否中招
### 设置代理，检测异常流量
	为了现身说法，装了一个中国联通网上营业厅的APP，Mac上开启Charles并设置好手机的代理，让我们来捕捉下流量。
	APP第一次启动，啥都没干，就偷偷地发送了5次请求，我也是醉了。
![中国联通网上营业厅](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/_images/xcodeghost_8.jpg)
![Charles异常流量](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/_images/xcodeghost_9.png)
### 分析ipa文件
	class-dump 方式:
	通过网络获得的微信6.2.5的ipa包，找到其.app文件，调用:
	class-dump --arch armv7 wechat/Payload/WeChat.app > ~/Desktop/wechat.apis
![微信6.2.5](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/_images/xcodeghost_6.png)
	
	由上图不难看到xcodeghost的特征函数。
	
	或者使用otool直接分析二进制文件的TEXT段，提取__cstring如下:
	otool -arch armv7 -v -s __TEXT __cstring wechat/Payload/WeChat.app/WeChat > wechat.strings
![微信6.2.5](https://github.com/kangwang1988/kangwang1988.github.io/raw/master/_images/xcodeghost_7.png)
	
	由上图不难看到xcodeghost的特征字符串http://init.icloud-analysis.com
## 意见与建议

	按理来说，我们要是做一些类似的不是坏事的实验(都不敢说好事...)，都是通过写个小Demo，然后都不发布去做的，以此事的发生轨迹，预谋已久，影响范围大，来看，我更愿意相信作者没那么纯洁,只是事情闹大了，很早地被发现了，不好收场而已，建议大家及时更新新版本，如果没有新版本，删掉等新版本修复。虽说后门服务器关掉了，可后门还在，随时都会有问题，大家还是要养成良好的信息安全习惯和意识，涉事单位厂商以及码农兄弟们也要小心了，这次的事情还好没有造成太大的后果，不然真是不敢想象，脊背发凉。。。
	


## 更多
Contact [KyleWong](mailto:kang.wang1988@gmail.com) for more.