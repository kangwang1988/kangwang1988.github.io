##引子

包瘦身，包瘦身，包瘦身，重要的事情说三遍。
最近公司一款iOS APP(本文只讨论使用Objective C开发的iOS安装包)一直在瘦身，我们团队的APP也愈发庞大了。而要解决这个问题，思路主要集中在两个方向，资源和代码。资源主要在于图片，方法包括移除未被引用的图片，只使用一套图片(2x或3x)，图片伸缩等；代码层面主要思路包括重构消除冗余，linkmap中selector引用分析等。除此之外，有没有别的路径呢？
众所周知，代码之间存在调用关系。假设iOS APP的主入口为-[UIApplication main],则所有开发者的源代码(包括第三方库)可分为两类:存在一条调用路径，使得代码可以被主入口最终调用(称此类代码为被最终调用)；不存在一条调用路径，使得代码最终不能被主入口调用(称此类代码为未被最终调用)。
假设有一个源代码级别的分析工具(或编译器)，可以辅助分析出代码间的调用关系，这样就使得分析最终被调用代码成为了可能，剩下的也就是未被最终调用的代码呢。
而这种工具目前有成熟可用的么，答案是肯定的，就是clang插件。除过分析未被最终调用代码，clang还可以辅助发现重复代码。

## LLVM与clang插件

LLVM工程包含了一组模块化，可服用的编辑器和工具链。同其名字原意(Low Level Virtual Machine)不同的是，LLVM不是一个首字母缩写，而是工程的名字。目前LLVM包含的主要子项目包括:
```markdown
1.LLVM Core:包含一个现在的源代码/目标设备无关的优化器，一集一个针对很多主流(甚至于一些非主流)的CPU的汇编代码生成支持。
2.Clang:一个C/C++/Objective-C编译器，致力于提供令人惊讶的快速编译，极其有用的错误和警告信息，提供一个可用于构建很棒的源代码级别的工具.
3.dragonegg: gcc插件，可将GCC的优化和代码生成器替换为LLVM的相应工具。
4.LLDB:基于LLVM提供的库和Clang构建的优秀的本地调试器。
5.libc++、libc++ ABI: 符合标准的，高性能的C++标准库实现，以及对C++11的完整支持。
6.compiler-rt:针对`__fixunsdfdi`和其他目标机器上没有一个核心IR(intermediate representation)对应的短原生指令序列时，提供高度调优过的底层代码生成支持。
7.OpenMP: Clang中对多平台并行编程的runtime支持。
8.vmkit:基于LLVM的Java和.NET虚拟机实
9.polly: 支持高级别的循环和数据本地化优化支持的LLVM框架。
10.libclc: OpenCL标准库的实现
11.klee: 基于LLVM编译基础设施的符号化虚拟机
12.SAFECode:内存安全的C/C++编译器
13.lld: clang/llvm内置的链接器
```

作为LLVM提供的编译器前端，clang可将用户的源代码(C/C++/Objective-C)编译成语言/目标设备无关的IR实现。并且提供良好的插件支持，容许用户在编译时，运行额外的自定义动作。

## 环境搭建 
### Clone clang源码并编译安装
```markdown
`cd /opt
`sudo mkdir llvm
`sudo chown `whoami` llvm
`cd llvm
`export LLVM_HOME=`pwd`

`git clone -b release_39 git@github.com:llvm-mirror/llvm.git llvm
`git clone -b release_39 git@github.com:llvm-mirror/clang.git llvm/tools/clang
`git clone -b release_39 git@github.com:llvm-mirror/clang-tools-extra.git llvm/tools/clang/tools/extra
`git clone -b release_39 git@github.com:llvm-mirror/compiler-rt.git llvm/projects/compiler-rt
	
`mkdir llvm_build
`cd llvm_build
`cmake ../llvm -DCMAKE_BUILD_TYPE:STRING=Release
`make -j`sysctl -n hw.logicalcpu`
```
### 编写clang插件(详见下文中自定义clang插件的描述)
```markdown
要实现自定义的clang插件(以C++ API为例)，应按照以下步骤:
1.自定义继承自
`clang::PluginASTAction`(基于consumer的AST前端Action抽象基类)
`clang::ASTConsumer`(用于客户读取AST的抽象基类)，
`clang::RecursiveASTVisitor`(前序或后续地深度优先搜索整个抽象语法树(AST)，并访问每一个节点的基类)等基类，
2.根据自身需要重载
`PluginASTAction::CreateASTConsumer`
`PluginASTAction::ParseArgs`
`ASTConsumer::HandleTranslationUnit`
`RecursiveASTVisitor::VisitDecl`
`RecursiveASTVisitor::VisitStmt`
等方法，实现自定义的分析逻辑。
3.注册插件
`static FrontendPluginRegistry::Add<MyPlugin> X("my-plugin-		  name", "my-plugin-description");`
```
[更多clang插件](http://clang.llvm.org/docs/ExternalClangExamples.html)
### 编译生成插件(dylib)
```markdown
`clang -std=c++11 -stdlib=libc++ -L/opt/local/lib -L/opt/llvm/llvm_build/lib -I/opt/llvm/llvm_build/tools/clang/include -I/opt/llvm/llvm_build/include -I/opt/llvm/llvm/tools/clang/include -I/opt/llvm/llvm/include -dynamiclib -Wl,-headerpad_max_install_names -lclang -lclangFrontend -lclangAST -lclangAnalysis -lclangBasic -lclangCodeGen -lclangDriver -lclangFrontendTool -lclangLex -lclangParse -lclangSema -lclangEdit -lclangSerialization -lclangStaticAnalyzerCheckers -lclangStaticAnalyzerCore -lclangStaticAnalyzerFrontend -lLLVMX86CodeGen -lLLVMX86AsmParser -lLLVMX86Disassembler -lLLVMExecutionEngine -lLLVMAsmPrinter -lLLVMSelectionDAG -lLLVMX86AsmPrinter -lLLVMX86Info -lLLVMMCParser -lLLVMCodeGen -lLLVMX86Utils -lLLVMScalarOpts -lLLVMInstCombine -lLLVMTransformUtils -lLLVMAnalysis -lLLVMTarget -lLLVMCore -lLLVMMC -lLLVMSupport -lLLVMBitReader -lLLVMOption -lLLVMProfileData -lpthread -lcurses -lz -lstdc++ -fPIC -fno-common -Woverloaded-virtual -Wcast-qual -fno-strict-aliasing -pedantic -Wno-long-long -Wall -Wno-unused-parameter -Wwrite-strings -fno-rtti -fPIC ./your-clang-plugin-source.cpp -o your-clang-plugin-name.dylib
```
### 与Xcode集成
[下载XcodeHacking.zip](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/others/XcodeHacking.zip)
```markdown
使用命令行编译时，可以用如下方式载入插件:
`clang++ *** -Xclang -load -Xclang path-of-your-plugin.dylib -Xclang -add-plugin -Xclang your-pluginName -Xclang -plugin-arg-your-pluginName -Xclang your-pluginName-param`
要在Xcode中使用clang插件，需要如下hack Xcode.
`sudo mv HackedClang.xcplugin `xcode-select -print-	path`/../PlugIns/Xcode3Core.ideplugin/Contents/SharedSupport/Developer/Library/Xcode/Plug-ins`
`sudo mv HackedBuildSystem.xcspec `xcode-select -print-path`/Platforms/iPhoneSimulator.platform/Developer/Library/Xcode/Specifications`
在Xcode->Target-Build Settings->Build Options->Compiler for C/C++/Objective-C选择Clang LLVM Trunk即可使得Xcode使用上文生成的的clang来编译。至于其他命令行参数均可通过Xcode中的编译选项设置完成。
```
## 代码级别包瘦身
本文所说的代码指的是OC中的形如-/+[Class method:\*]这种形式的代码，调用关系典型地如:
```markdown
`@interface ViewController : UIViewController
`@end
`@implementation ViewController
`- (void)viewDidLoad {
`  [super viewDidLoad];
`  [self.view setBackgroundColor:[UIColor redColor]];
`}
`@end
则称:`-[ViewController viewDidLoad]`调用了:
`-[UIViewController viewDidLoad]`
`-[ViewController view]`(语法糖)
`+[UIColor redColor]`
`-[UIView setBackgroundColor:]`
```
### 步骤
```markdown
1. 编写clang插件(将生成供分析工具分析的中间文件)
2. 编写需要分析的工程
3. 使用用户编译的clang，并载入步骤1生成的插件去编译步骤2中的源文件并生成各种中间文件
4. 编写分析工具去分析各种中间文件并生成最终结果
```
### 数据结构

为了分析调用关系，需要生成的中间文件数据结构如下:

#### 类接口与继承体系(clsInterfHierachy)

此数据结构记录了所有位于抽象语法树(Abstract Syntax Tree/AST)上的接口内容，最终的解析结果如下图所示:

![clang-find-duplicate-unused-code-clsInterfHierachy](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/clang-find-duplicate-unused-code-clsInterfHierachy.png)

以AppDelegate为例，interfs代表其提供的接口(注:它的property window对应的getter和setter也被认为是interf一部分);isInSrcDir代表此类是否位于用户目录(将workspace的根目录作为参数传给clang)下，protos代表其遵守的协议，superClass代表接口的父类。
```markdown
这些信息获取入口位于`VisitDecl(Decl \*decl)`的重载函数里，相关的decl有`ObjCInterfaceDecl`(接口声明)
`ObjCCategoryDecl`(分类声明)
`ObjCPropertyDecl`(属性声明)
`ObjCMethodDecl`(方法声明)
```
#### 接口方法调用(clsMethod)

此数据结构记录了所有包含源代码的OC方法，最终解析结果如下所示:

![clang-find-duplicate-unused-code-clsMethod](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/clang-find-duplicate-unused-code-clsMethod.png)
```markdown
以`-[AppDelegate application:didFinishLaunchingWithOptions:]`为例,callee代表其调用到的接口(此处为可以明确类型的，对于形如`id\<XXXDelegate\>`后文介绍)，filename为此方法所在的文件名，range为方法所在的范围，sourceCode为方法的具体实现源代码。
这些信息获取入口位于`VisitDecl(Decl \*decl)`和`VisitStmt(Stmt \*stmt)`的重载函数里，相关的decl有
`ObjCMethodDecl`(方法声明)，stmt有`ObjCMessageExpr`(消息表达式)
此处除过正常的`-/+[Class method:\*]`外，还有其他较多的需要考虑的情形，已知且支持的分析包括:
NSObject协议的performSelector方法簇，
`[obj performSelector:@selector(XXX)]`不仅包含`[obj performSelector:]`也包含`[obj XXX]`.(下同)
手势/按钮的事件处理selector
`addTarget:action:/initWithTarget:action:/addTarget:action:forControlEvents:`
NSNotificationCener添加通知处理Selector
`addObserver:selector:name:object:`
UIBarButtonItem添加事件处理Selector
`initWithImage:style:target:action:/initWithImage:landscapeImagePhone:style:target:action:/initWithTitle:style:target:action:/initWithBarButtonSystemItem:target:action:`
Timer
`scheduledTimerWithTimeInterval:target:selector:userInfo:repeats:/timerWithTimeInterval:target:selector:userInfo:repeats:/initWithFireDate:interval:target:selector:userInfo:repeats:`
NSThread
`detachNewThreadSelector:toTarget:withObject:/initWithTarget:selector:object:`
CADisplayLink
`displayLinkWithTarget:selector:`
KVO机制
`addObserver:forKeyPath:options:context:`,不同于别的都要处理方法本身调用和对应target:selector调用，这里KVO的addObserver则暗含了`observeValueForKeyPath:ofObject:change:context:`。
`[XXX new]`
包含`+[XXX alloc]`和`-[XXX init]`。
```
####协议的接口与继承体系(protoInterfHierachy)
此数据结构记录了所有位于AST(抽象语法树)上的协议内容，最终的解析结果如下图所示:

![clang-find-duplicate-unused-code-protoInterfHierachy](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/clang-find-duplicate-unused-code-protoInterfHierachy.png)

```markdown
其中各字段定义同clsInterfHierachy.
这些信息获取入口位于`VisitDecl(Decl \*decl)`的重载函数里，相关的decl有`ObjCProtocolDecl`(协议声明)
`ObjCPropertyDecl`(属性声明)
`ObjCMethodDecl`(方法声明)
```

####协议方法的调用(protoInterfCall)
```markdown
此数据结构记录了所有如:`-[ViewController func1]`调用了`-[id\<ViewControllerDelegate\> viewController:execFunc:]`的形式，最终结果如下所示:
```
![clang-find-duplicate-unused-code-protointerfcall](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/clang-find-duplicate-unused-code-protointerfcall.png)
``` markdown
这些信息获取入口位于`VisitStmt(Stmt \*stmt)`的重载函数里，相关的stmt是`ObjCMessageExpr`.
```
#### 添加通知

以第一条记录为例，其意思是说-[AppDelegate onViewControllerDidLoadNotification:]作为通知kNotificationViewControllerDidLoad的Selector，在-[AppDelegate application:didFinishLaunchingWithOptions:]中被添加。

![clang-find-duplicate-unused-code-clsMethodAddNotifs](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/clang-find-duplicate-unused-code-clsMethodAddNotifs.png)

#### 发送通知
```markdown
第一条记录中，作为系统级别的通知，将被认为被APP主入口调用。
第二条记录则说明了，`-[ViewController viewDidLoad]`发送了kNotificationViewControllerDidLoad。
```
![clang-find-duplicate-unused-code-notifPostedCallers](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/clang-find-duplicate-unused-code-notifPostedCallers.png)

```markdown
如果`-[AppDelegate application:didFinishLaunchingWithOptions:]`被`-[UIApplication main]`(假定的主入口)调用，且`-[ViewController viewDidLoad]`被调用，则`-[AppDelegate onViewControllerDidLoadNotification:]`被调用。其中，如果通知是系统通知，则只需要`-[AppDelegate application:didFinishLaunchingWithOptions:]`被调用即可。

这些信息获取入口位于`VisitStmt(Stmt \*stmt)`的重载函数里，相关的stmt有`ObjCMessageExpr`.为了简单处理，此处只处理形如`addObserver:self`这种(也是最常见的情况)，否则Argu作为`Expr\*`分析起来会很复杂。PS.系统通知和本地通知的区别使用了名称上的匹配(系统通知常以NS,UI,AV开头以Notification结束).
```
## 重复代码分析

​	此处的重复代码针对的是某两个(或两个以上)-/+[Class method:\*]的实现是一模一样的。参考上文提到的clsMethod中的sourceCode，可以获得每一个方法实现的源代码。同时为了消除诸如格式上的差异(如多了一个空格，少了一个空格之类)引起的差异，先基于clang提供的format功能，按照某种风格(google/llvm等)将所有方法实现源码格式化，再进行分析即可。

​	使用LLVM风格将代码format:

```markdown
`find $prjDir -type f -name "\*.m" | xargs /opt/llvm/llvm_build/bin/clang-format -i -style=LLVM`
```

本文示例工程得到的一个重复代码结果如下所示:
![clang-find-duplicate-unused-code-repeatCode](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/clang-find-duplicate-unused-code-repeatCode.png)

## 未被最终调用代码分析
```markdown
分析的对象在于clsMethod.json里面所有的key，即实际拥有源代码的所有方法。
1.初始化默认的调用关系usedClsMethodJson:`{-[AppDelegate alloc],"-[UIApplication main]","-[UIApplication main]","-[UIApplication main]","+[NSObject alloc]","-[UIApplication main]"}`,其中AppDelegate由用户传给Analyzer.
2.分析所有含源码方法是否存在一条路可以被已经调用usedClsMethodJson中的key调用。
	对于某一个clsMethod，其需要检查的路径包括三个，类继承体系，协议体系和通知体系。
	a.针对类继承体系，从当前类一直向上追溯(直到发现有被调用或者NSObject)，每一个基类对应的`-/+[Class method:*]`是否被隐含的调用关系所调用，如`-[ViewController viewDidLoad]`被`-[ViewController alloc]`隐含调用，当`-[ViewController alloc]`已经被调用的时候，`-[ViewController viewDidLoad]`也将被认为调用。这里需要注意需要写一个隐含调用关系表以供查询，如下所示:
```
![clang-find-duplicate-unused-code-implicitCallStackJson](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/clang-find-duplicate-unused-code-implicitCallStackJson.png)
```markdown
	b.针对Protocol体系，需要参考类似Protocol引用体系向上追溯(直到发现有被调用或者`NSObject`协议)，针对某一个特定的Protocol判断的时候，需要区分两种，一种是系统级的Protocol，如`UIApplicationDelegate`，对于`-[AppDelegate application:didFinishLaunchingWithOptions:]`这种，参考`AppDelegate<UIApplicationDelegate>`，如果`-[AppDelegate alloc]`被调用则认为`-[AppDelegate application:didFinishLaunchingWithOptions:]`被调用。针对用户定义的Protocol,如`ViewControllerDelegate`，对于`-[AppDelegate viewController:execFunc:]`不仅需要`-[AppDelegate alloc]`被调用并且protoInterfCall.json中`-[ViewControllerDelegate viewController:execFunc:]`对应的Callers有已经存在于usedClsMethodJson的Caller.
```
![clang-find-duplicate-unused-code-protointerfcall](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/clang-find-duplicate-unused-code-protointerfcall.png)

针对通知体系，前文已经有过分析。
一个简单的分析结果如下图:
使用到的ClsMethod

![clang-find-duplicate-unused-code-usedclsmethod](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/clang-find-duplicate-unused-code-usedclsmethod.png)
未使用到的ClsMethod

![clang-find-duplicate-unused-code-unusedclsmethod](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/clang-find-duplicate-unused-code-unusedclsmethod.png)

[查看示例工程](https://github.com/kangwang1988/XcodeZombieCode.git)

## 局限
```markdown
这种静态分析适合可以判断出消息接收者类型的情况，面对运行时类型和静态分析类型不一致，或者静态分析不出来类型时，不可用。
这种分析要求代码书写规范。例如一个Class实现了某个Protocol，一定要在声明里说明，或者Property中delegate是`id<XXXDelegate>`的时候也要注明。
```
## 个性化定制
虽然此项目已经给了一个完整的重复代码和无用代码分析工具，但也有其局限性(主要是动态特性)。具体分析如下:
```markdown
1.openUrl机制
假设工程设置里使用了`openUrl:"XXX://XXViewController"`来打开一个VC，则Clang插件里面需要分析openUrl的参数，如果参数是XXViewController，则暗含了`+[XXViewController alloc]`和`-[XXViewController init]`.
2.Model转化
如如果MTLModel使用到了`modelOfClass:[XXXModel class] fromJSONDictionary:error:`，则暗含了`+[XXXModel alloc]`和`+[XXXModel init]`.
3.message swizzle
假设用户swizzle了`-[UIViewController viewDidLoad]`和`-[UIViewController XXviewDidLoad]`，则需要在implicitCallStackJson中添加`-[UIViewController XXviewDidLoad]`,`-[UIViewController viewDidLoad]`.
4.第三方Framework暗含的逻辑
如高德地图的AnnotationView,需要implicitCallStackJson中添加`"-[MAAnnotationView prepareForReuse:]","+[MAAnnotationView alloc]"`等。包括第三方Framework里面的一些Protocol，可能也需要参考前文提到的UIApplicationDelegate按照系统级别的Protocol来处理。
5.一些遗漏的重载方法
如`-[XXDerivedManager sharedInstance]`并无实现，而XXDerivedManager的基类XXBaseManager的sharedInstance调用了`-[self alloc]`,但因为self静态分析时被认定为XXBaseManager，这就导致`-[XXDerivedManager sharedManager]`虽然被usedclsmethod.json调用，但是`-[XXDerivedManager alloc]`却不能被调用。这种情况，可以在usedClsMethodJson初始化的时候，加入 `"+[XXDerivedManager alloc]","-[UIApplication main]"`。
6.类似Cell Class
我们常会使用动态的方法去使用`[[[XXX cellClassWithCellModel:] alloc] initWithStyle:reuseIdentifier:]`去构造Cell，这种情况下，应该针对`cellClassWithCellModel`里面会包含的各种`return [XXXCell class]`，在implicitCallStackJson中添加`[[XXXCell alloc] initWithStyle:reuseIdentifier:],-[XXX cellClassWithCellModel:]`这种调用。
7.其他隐含的逻辑或者动态特性导致的调用关系遗漏。
```
## 其他
```markdown
对于包大小而言，可以参考以下的思路去瘦身代码:
	1.重复代码的提取重构
	2.无用代码的移除
	3.使用率较低的第三方库的处理(本文不仅可以查找到重复，无用的代码，进一步分析clsMethod.json/unusedClsMethod.json更可以获取到每一个framework里面有多少个方法，各方法有多少代码，多少个方法又被`-[UIApplication main]`调用到了)，面对使用率很低的库，需要考虑是不是要全部引入或者重写。
	4.重复引用的第三方库的处理(曾经发现团队项目的工程里面引用了其他团队的库，但由于多个库里面均有一份自己的Zip的实现，面对这种情况，可以考虑将此种需求全部抽象出来一个公共的Framework去处理，其他人都引用此项目，或者干脆使用系统本身自带的libz去处理会更好些)。
```
使用clang 插件，