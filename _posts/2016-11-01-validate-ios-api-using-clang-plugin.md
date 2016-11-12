---
layout: post
comments: true
section-type: post
title: Clang技术分享系列三:API有效性检查
category: tech
tags: [ 'clang' ]

---
## 问题背景

       iOS API(Class/Protocol的Interface，Property，Const，Function)的start/deprecated的判断依赖两个方面:
       1.获得所有的API调用，包括OC消息发送，C API调用，常量使用等；
       2.获取所有的API支持版本信息
       将1中的API调用，在2中做搜索比对，并结合APP的deploy target和最新的支持系统，判断API的有效性，如果API在deploy target之后开始或者在当前系统之前(包括当前系统)废弃，则提示用户。
       第二个问题可以参见我的另一篇blog(XCODE8 API文档解析),本文主要介绍问题1.

[XCODE8 API文档解析](http://kangwang1988.github.io/tech/2016/10/31/xcode8-documented-api-analyzer.html)

## 分析APP源代码API数据结构准备

    1.类接口以及继承体系(clsInterfHierachy)
    此数据结构记录了所有位于AST(抽象语法树)上的接口内容，最终的解析结果如下图所示:

![clang-validate-ios-api-clsInterfHierachy](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/clang-validate-ios-api-clsInterfHierachy.png)

	以AppDelegate为例，interfs代表其提供的接口(注:他的property window对应的getter和setter也被认为是interf一部分);isInSrcDir代表此类是否位于用户目录(将workspace的根目录作为参数传给clang)下，protos代表其conform的协议，superClass代表接口的父类。
	这些信息获取入口位于VisitDecl(Decl *decl)的重载函数里，相关的decl有ObjCInterfaceDecl,ObjCCategoryDecl,ObjCPropertyDecl,ObjCMethodDecl.
	
	2.接口方法调用(clsMethod)
	此数据结构记录了所有包含源代码的OC方法，最终解析结果如下所示:

![clang-validate-ios-api-clsMethod](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/clang-validate-ios-api-clsMethod.png)

	以-[AppDelegate application:didFinishLaunchingWithOptions:]为例,callee代表其调用到的接口(此处为可以明确类型的，对于形如id<XXXDelegate>随后介绍)，filename为此方法所在的文件名，range为方法所在的范围，sourceCode为方法的具体实现源代码。
	这些信息获取入口位于VisitDecl(Decl *decl)和VisitStmt(Stmt *stmt)的重载函数里，相关的decl有ObjCMethodDecl,stmt有ObjCMessageExpr.
	此处除过正常的-/+[Cls message]为，还有其他较多的需要考虑的情形，已知且支持的分析包括:
	NSObject协议的performSelector方法簇，[obj performSelector:@selector(XXX)]不仅代表了[obj performSelector:]也代表了[obj XXX].(下同)
	手势/按钮的事件处理selector
	addTarget:action:/initWithTarget:action:/addTarget:action:forControlEvents:
	NSNotificationCener添加通知处理Selector
	addObserver:selector:name:object:
	UIBarButtonItem添加事件处理Selector
	initWithImage:style:target:action:/initWithImage:landscapeImagePhone:style:target:action:/initWithTitle:style:target:action:/initWithBarButtonSystemItem:target:action:
	Timer
	scheduledTimerWithTimeInterval:target:selector:userInfo:repeats:/timerWithTimeInterval:target:selector:userInfo:repeats:/initWithFireDate:interval:target:selector:userInfo:repeats:
	NSThread
	detachNewThreadSelector:toTarget:withObject:/initWithTarget:selector:object:
	CADisplayLink
	displayLinkWithTarget:selector:
	KVO机制
	IBAction机制
	如基于xib/Storyboard的ViewController中`-(IBAction)onBtnPressed:(id)sender`方法，认为暗含了`+[ViewController的 alloc]`对于`+[ViewController的 onBtnPressed:]`的调用关系。
	
	[XXX new]
	暗含了+[XXX alloc]和-[XXX init]
	
	3.类Property对应的getter/setter(clsPropertyGS)
	此数据结构记录了所有类的property与getter/setter之间的对应关系，最终结果如下所示:

![clang-validate-ios-api-clsPropertyGS](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/clang-validate-ios-api-clsPropertyGS.png)

    这些信息获取入口位于VisitDecl(Decl *decl)的重载函数里，相关的decl有ObjCPropertyDecl.

    4.变量/枚举/变量等(funcEnumVar)
    此数据结构记录了所有使用到的函数，枚举和变量，最终结果如下所示:

![clang-validate-ios-api-funcEnumVar](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/clang-validate-ios-api-funcEnumVar.png)

	5.协议的接口与继承体系(protoInterfHierachy)
	此数据结构记录了所有位于AST(抽象语法树)上的协议内容，最终的解析结果如下图所示:

![clang-validate-ios-api-protoInterfHierachy](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/clang-validate-ios-api-protoInterfHierachy.png)
​	
	其中各字段定义同clsInterfHierachy.
	这些信息获取入口位于VisitDecl(Decl *decl)的重载函数里，相关的decl有ObjCProtocolDecl,ObjCPropertyDecl,ObjCMethodDecl.
	
	6.协议Property对应的getter/setter(protoPropertyGS)
	此数据结构记录了所有协议的property与getter/setter之间的对应关系，最终结果如下所示:

![clang-validate-ios-api-protoPropertyGS](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/clang-validate-ios-api-protoPropertyGS.png)

    这些信息获取入口位于VisitDecl(Decl *decl)的重载函数里，相关的decl有ObjCPropertyDecl.

	7.协议方法的调用(protoInterfCall)
	此数据结构记录了所有如:-[SiriUIViewController viewDidLoad]调用了-[id<INIntentHandlerProviding> handlerForIntent:]的形式，最终结果如下所示:

![clang-validate-ios-api-protoInterfCall](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/clang-validate-ios-api-protoInterfCall.png)

    这些信息获取入口位于VisitStmt(Stmt *stmt)的重载函数里，相关的stmt是ObjCMessageExpr.

## 处理过的苹果官方文档

    此处过程可参见上文提到的我的另一篇博客XCODE8 API文档解析，此处只截图说明下处理过的各种API对应版本支持信息的数据结构。

![clang-validate-ios-api-vercls](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/clang-validate-ios-api-vercls.png)

![clang-validate-ios-api-verClsInterfs](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/clang-validate-ios-api-verClsInterfs.png)

![clang-validate-ios-api-verClsProperty](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/clang-validate-ios-api-verClsProperty.png)

![clang-validate-ios-api-verConst](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/clang-validate-ios-api-verConst.png)

![clang-validate-ios-api-verFunc](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/clang-validate-ios-api-verFunc.png)

![clang-validate-ios-api-verProto](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/clang-validate-ios-api-verProto.png)

![clang-validate-ios-api-verProtoInterfs](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/clang-validate-ios-api-verProtoInterfs.png)

![clang-validate-ios-api-verProtoProperty](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/clang-validate-ios-api-verProtoProperty.png)

## 最终分析

	对于变量，枚举，函数等，只需要拿到名称之后到对应的verXXX.json去比对，如果发现了匹配，提取其对应的支持版本信息，并与传入clang的deployTarget和当前最新支持的系统作比较，根据需要确定是否输出警告。
	对于类，需要在其继承层次上去查看每一个基类的支持版本信息，做判断，直到到达NSOBject或者已经有警告为止；
	对于-/+[Cls Sel]这种，首先需要从clsMethod.json中提取所有的-/+[Cls Sel]调用(包括调用者和被调用者)，然后针对每一个-/+[Cls Sel]，沿着两个维度去搜索。一个是类继承体系(一直到已经存在的匹配方法/追溯到到NSObject类/遇到警告)，一个是Protocol引用体系(一直到已经存在的匹配方法/追溯到到NSObject协议/遇到警告)。这两个维度里，判断的时候针对-/+[Cls Sel]既要考虑Sel可能是一个Sel，也可能是一个Property，尝试Property时需把Sel隐射到property上(使用clsPropertyGS)，再同VerClsProperty比较方可。
	对于-/+[Protocol Sel]这种，首先从protoInterfCall.json提取所有的-/+[Protocol Sel](只有被调用，调用者已在-/+[Cls Sel]有处理)。然后沿着Protocol引用体系去处理，处理时同-/+[Cls Sel]，需要考虑Sel和Property两个可能。
	一个简单的分析结果如下图:

![clang-validate-ios-api-warnCls](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/clang-validate-ios-api-warnCls.png)

![clang-validate-ios-api-warnClsSel](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/clang-validate-ios-api-warnClsSel.png)

![clang-validate-ios-api-warnProtoSel](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/clang-validate-ios-api-warnProtoSel.png)

[检出示例工程](https://github.com/kangwang1988/XcodeValidAPI.git)

## 局限

	说到底这种静态的分析适合的时比较容易判断出消息接收者类型的例子，面对静态分析的类型和实际不一致，或者静态分析不出来类型的时候，是无能为力的。
	此外，这种分析，对于代码的书写规范有要求。例如一个Class实现了某个Protocol，一定要在声明里说明，或者Property中delegate是id<XXXDelegate>的时候也要注明。当然，这种规范的工作本来就是应该的。

## 如何与Xcode集成

    现有的机制是
    1.书写ClangPlugin
    2.书写分析可执行文件
    3.使用用户编译的Clang载入ClangPlugin去编译并生成各种中间文件。
    4.Build结束的时候，使用Xcode提供的post_build_action_shell机制调用分析工具，生成最终结果。
    
    如果需要与Xcode集成，那么利用4之后生成的结果文件，结合 Clang技术分享系列二:代码风格检查提到的FixItHint功能，在Xcode中提示用户。

[Contact me](mailto:kang.wang1988@gmail.com)