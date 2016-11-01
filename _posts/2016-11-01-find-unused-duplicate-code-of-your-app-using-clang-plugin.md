---
layout: post
comments: true
section-type: post
title: Clang技术分享系列四:iOS APP无用代码/重复代码分析
category: tech
tags: [ 'clang' ]

---
## 问题背景

    包瘦身，包瘦身，包瘦身，重要的事情说三遍。
    最近公司一款APP一直在瘦身，我们团队的APP也愈发庞大了。就想看看除过资源外，还有那些路径可以缩小包大小，直观来看，业务代码毕竟有限，各种库嫌疑很大，但是如果没有一个定量的分析，一切说辞都显得有些苍白。
    当然了所有的APP套路都一样，开始运行就跑一个循环，不断地从消息队列里去获取消息，获取到用户操作，系统通知等消息的时候就处理此消息，获取不到就休息。一直循环往复到被杀死，说到底所有的有用API都应该有一条最终被main函数调用的路径，否则就认为是无用的代码。

继续阅读之前，请先阅读我的另一篇博文Clang技术分享系列三:API有效性检查

[Clang技术分享系列三:API有效性检查](http://kangwang1988.github.io/tech/2016/11/01/validate-ios-api-using-clang-plugin.html)

## 分析无用代码数据结构

    除过系列三提到的clsInterfHierachy，clsMethod，protoInterfHierachy，protoInterfCall外还用到的数据结构有:
    1.clsMethod添加通知
    以第一条记录为例，其意思是说-[AppDelegate onViewControllerDidLoadNotification:]作为通知kNotificationViewControllerDidLoad的Selector，在-[AppDelegate application:didFinishLaunchingWithOptions:]中被添加。

![clang-validate-ios-api-clsMethodAddNotifs](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/clang-validate-ios-api-clsMethodAddNotifs.png)

	2.clsMethod发送通知
	以第二条记录为例，其代表了-[ViewController viewDidLoad]发送了kNotificationViewControllerDidLoad。

![clang-validate-ios-api-notifPostedCallers](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/clang-validate-ios-api-notifPostedCallers.png)

	这样对于通知，如果-[AppDelegate application:didFinishLaunchingWithOptions:]被-[UIApplication main](假定的主入口)调用，且-[ViewController viewDidLoad]被调用，则-[AppDelegate onViewControllerDidLoadNotification:]被调用。其中，如果通知是系统通知，则只需要-[AppDelegate application:didFinishLaunchingWithOptions:]被调用即可。

	这些信息获取入口位于VisitStmt(Stmt *stmt)的重载函数里，相关的stmt有ObjCMessageExpr.为了简单处理，此处只处理形如addObserver:self这种(也是最常见的情况)，否则Argu作为Expr*分析起来会很复杂。PS.系统通知和本地通知的区别使用了名称上的匹配(系统通知常以NS,UI,AV开头以Notification结束).

## 最终分析

	如API有效性检查中提到的现有机制:书写Plugin->书写分析文件->使用Plugin去编译工程并生成中间文件->Build结束的时候，使用shell调用分析工具分析。分析工具现在着重两个方面重复代码和无用代码:
	1.重复代码的比对
	如API有效性检查中接口方法调用中提到的clsMethod的数据结构，可以通过clang-format掉所有clsMethod的源代码，然后hash求值，然后hash值一样的clsMethod将具有相同的源代码。
	本文使用的例子产生的结果如下所示:

![clang-validate-ios-api-repeatCode](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/clang-validate-ios-api-repeatCode.png)
​	
	2.无用代码的分析
	分析的对象在于clsMethod.json里面所有的key，即实际拥有源代码的-/+[cls method]调用。
	a.初始化默认的调用关系usedClsMethodJson:-[AppDelegate alloc],"-[UIApplication main]","-[UIApplication main]","-[UIApplication main]","+[NSObject alloc]","-[UIApplication main]",其中AppDelegate由用户传给Analyzer.
	b.分析-/+[cls method]是否存在一条路可以被已经调用usedClsMethodJson中的key调用。
	对于某一个clsMethod，其需要检查的路径包括三个，继承体系，Protocol体系和Notification体系。
	针对Notification体系，前文已经有过分析。
	针对类继承体系，从当前类一直向上追溯(直到发现有被调用或者NSObject)，每一个基类对应的-/+[cls method]是否被隐含的调用关系所调用，如-[ViewController viewDidLoad]被-[ViewController alloc]隐含调用，当-[ViewController alloc]已经被调用的时候，-[ViewController viewDidLoad]也将被认为调用。这里需要注意需要写一个隐含调用关系表以供查询，如下所示:

![clang-find-duplicate-unused-code-implicitCallStackJson]https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/clang-find-duplicate-unused-code-implicitCallStackJson.png
​	
​	
	针对Protocol体系，需要参考类似Protocol引用体系向上追溯(直到发现有被调用或者NSObject协议)，针对某一个特定的Protocol判断的时候，需要区分两种，一种是系统级的Protocol，如UIApplicationDelegate，对于-[AppDelegate application:didFinishLaunchingWithOptions:]这种，因为AppDelegate<UIApplicationDelegate>，如果-[AppDelegate alloc]被调用则直接认为-[AppDelegate application:didFinishLaunchingWithOptions:]被调用。针对用户定义的Protocol,如ViewControllerDelegate，对于-[AppDelegate viewController:execFunc:]不仅需要-[AppDelegate alloc]被调用并且protoInterfCall.json中-[ViewControllerDelegate viewController:execFunc:]对应的Callers有已经存在于usedClsMethodJson的Caller.

![clang-find-duplicate-unused-code-protointerfcall](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/clang-find-duplicate-unused-code-protointerfcall.png)
​	
	一个简单的分析结果如下图:
	使用到的ClsMethod

![clang-find-duplicate-unused-code-usedclsmethod](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/clang-find-duplicate-unused-code-usedclsmethod.png)

	未使用到的ClsMethod

![clang-find-duplicate-unused-code-unusedclsmethod](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/clang-find-duplicate-unused-code-unusedclsmethod.png)

[检出示例工程](https://github.com/kangwang1988/XcodeZombieCode.git)

## 针对不同工程的定制

	虽然此项目已经给了一个完整的重复代码和无用代码分析工具，但也有其局限性(主要是动态特性)。具体分析如下:
	1.openUrl机制
	假设工程设置里使用了openUrl:"XXX://XXViewController"来打开一个VC或者模块，那么Clang插件里面需要分析openUrl的参数，如果参数是XXViewController，则暗含了+[XXViewController alloc]和-[XXViewController init].
	2.Model转化
	如如果MTLModel使用到了modelOfClass:[XXXModel class] fromJSONDictionary:error:，则暗含了+[XXXModel alloc]和+[XXXModel init]
	3.message swizzle
	假设用户swizzle了[UIViewController viewDidLoad]和[UIViewController XXviewDidLoad]，则需要在implicitCallStackJson中添加[UIViewController XXviewDidLoad],[UIViewController viewDidLoad]
	4.第三方Framework暗含的逻辑
	如高德地图的AnnotationView,需要implicitCallStackJson中添加"-[MAAnnotationView prepareForReuse:]","+[MAAnnotationView alloc]"等。包括第三方Framework里面的一些Protocol，可能也需要参考前文提到的UIApplicationDelegate按照系统级别的Protocol来处理。
	5.一些遗漏的重载方法
	如-[XXDerivedManager sharedInstance]并无实现，而XXDerivedManager的基类XXBaseManager的sharedInstance调用了-[self alloc],但因为self静态分析时被认定为XXBaseManager，这就导致-[XXDerivedManager sharedManager]虽然被usedclsmethod.json调用，但是-[XXDerivedManager alloc]却不能被调用。这种情况，可以在usedClsMethodJson初始化的时候，加入 "+[XXDerivedManager alloc]","-[UIApplication main]"。
	6.类似Cell Class
	我们常会使用动态的方法去使用[[[XXX cellClassWithCellModel:] alloc] initWithStyle:reuseIdentifier:]去构造Cell，这种情况下，应该针对cellClassWithCellModel里面会包含的各种return [XXXCell class]，在implicitCallStackJson中添加[[XXXCell alloc] initWithStyle:reuseIdentifier:],-[XXX cellClassWithCellModel:]这种调用。
	其他隐含的逻辑或者动态特性导致的调用关系遗漏。
## 其他问题

	正如API有效性检查一文提到的，分析工具要求代码书写要规范。并且对于很多只有运行时才能知道类型的问题无能为力。
	对于包大小而言，主要可以参考以下的思路去瘦身代码:
	1.重复代码的提取重构
	2.无用代码的移除
	3.使用率较低的第三方库的处理(本例不仅可以查找到重复，无用的代码，进一步分析clsMethod.json,unusedClsMethod.json更可以获取到每一个framework里面有多少个方法，多少个又被-[UIApplication main]调用到了)，面对使用率很低的库，需要考虑是不是要全部引入或者重写。
	4.重复引用的第三方库的处理(曾经发现团队项目的工程里面引用了其他团队的库，但由于多个库里面均有一份自己的Zip的实现，面对这种情况，可以考虑将此种需求全部抽象出来一个公共的Framework去处理，其他人都引用此项目，或者干脆使用系统本身自带的libz去处理会更好些)。

[Contact me](mailto:kang.wang1988@gmail.com)