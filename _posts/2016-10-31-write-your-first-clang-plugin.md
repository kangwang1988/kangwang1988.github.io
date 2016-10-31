---
layout: post
comments: true
section-type: post
title: Clang技术分享系列一:编写你的第一个Clang插件
category: tech
tags: [ 'clang' ]
---
## 引子

       以前遇到一个需求，检测iOS App代码中使用到的API是否存在开始支持的系统版本高于当前deploy target，或已经在当前支持的最新系统之前(包括当前系统)已经被标记为弃用(deprecated)，以免发生Crash或者代码不能被调到的情况。
       这个问题可以看成时两个子问题:
       1.获取到所有的API的调用
       2.拿到所有的Documented公共API的(起始版本，废弃版本)记录.
       因OC:-[UIViewController viewDidLoad]的处理逻辑和C:CFNotificationCenterRemoveObserver或者C++ API等类似，而iOS开发中使用的主要是OC，此处只重点处理第一种情况(实际上也支持C函数调用)。
       为了获取到所有API的调用，就需要分析源代码(二进制相关记录中是基于selector的，而我们需要的是-/+[cls msg]).这就不得的不提到大名鼎鼎的llvm和clang了。
       Clang系列将推出多篇文章，基于Xcode和Clang，实现诸如代码风格规范，有效API检查，无用代码筛查等特性，欢迎关注和讨论。



## LLVM & Clang

​	LLVM工程包含了一组模块化，可复用的编译器和工具链。和其名字的原意(Low level virtual machine)不同的是,LLVM不是一个首字母缩写，而是工程的名字。

​	目前LLVM的主要子项目包括:



	1.LLVM Core:包含一个现在的源代码/目标设备无关的优化器，一集一个针对很多主流(甚至于一些非主流)的CPU的汇编代码生成支持。
	2.Clang:一个C/C++/Objective-C编译器，致力于提供令人惊讶的快速编译，极其有用的错误和警告信息，提供一个可用于构建很棒的源代码级别的工具.
	3.dragonegg: gcc插件，可将GCC的优化和代码生成器替换为LLVM的相应工具。
	4.LLDB:基于LLVM提供的库和Clang构建的优秀的本地调试器。
	5.libc++、libc++ ABI: 符合标准的，高性能的C++标准库实现，以及对C++11的完整支持。
	6.compiler-rt:针对"__fixunsdfdi"和其他目标机器上没有一个核心IR(intermediate representation)对应的短原生指令序列时，提供高度调优过的底层代码生成支持。
	7.OpenMP: Clang中对多平台并行编程的runtime支持。
	8.vmkit:基于LLVM的Java和.NET虚拟机实现
	9.polly: 支持高级别的循环和数据本地化优化支持的LLVM框架。
	10.libclc: OpenCL标准库的实现
	11.klee: 基于LLVM编译基础设施的符号化虚拟机
	12.SAFECode:内存安全的C/C++编译器
	13.lld: clang/llvm内置的链接器

[点击了解更多](http://llvm.org)

[点击下载相关源代码](https://github.com/llvm-mirror/llvm)

## Clang插件支持

​        Clang作为LLVM提供的编译器前端，将用户的源代码(C/C++/Objective-C)编译成语言/目标设备无关的IR实现。并且提供良好的插件支持，容许用户在编译时，运行额外的自定义动作。

	要实现自定义的Clang插件(以C++为例)，应按照以下步骤:
	1.自定义继承自
			clang::PluginASTAction(基于consumer的AST前端Action抽象基类)
			clang::ASTConsumer(用于客户读取AST的抽象基类)，
			clang::RecursiveASTVisitor(前序或后续地深度优先搜索整个    	   AST，并访问每一个节点的基类)等基类，
	2.根据自身需要重载
			PluginASTAction::CreateASTConsumer，
		    PluginASTAction::ParseArgs,
	        ASTConsumer::HandleTranslationUnit
	        RecursiveASTVisitor::VisitDecl
			RecursiveASTVisitor::VisitStmt
	   等方法，实现自定义的分析逻辑。
	3.注册插件
		static FrontendPluginRegistry::Add<MyPlugin> X("my-plugin-		  name", "my-plugin-description");
	4.编译时载入插件
		clang++ *** -Xclang -load -Xclang path-of-your-		
		plugin.dylib -Xclang -add-plugin -Xclang your-pluginName 
		-Xclang -plugin-arg-your-pluginName -Xclang your-
		pluginName-param

注:AST->Abstract Syntax Tree抽象语法树
[更多clang插件](http://clang.llvm.org/docs/ExternalClangExamples.html)

## 例子(分析某.m文件中用户定义的OC类)
### 环境搭建
  1.下载clang相关源代码并编译



	cd /opt
	sudo mkdir llvm
	sudo chown `whoami` llvm
	cd llvm
	export LLVM_HOME=`pwd`
	
	git clone -b release_39 git@github.com:llvm-mirror/llvm.git llvm
	git clone -b release_39 git@github.com:llvm-mirror/clang.git llvm/tools/clang
	git clone -b release_39 git@github.com:llvm-mirror/clang-tools-extra.git llvm/tools/clang/tools/extra
	git clone -b release_39 git@github.com:llvm-mirror/compiler-rt.git llvm/projects/compiler-rt
	
	mkdir llvm_build
	cd llvm_build
	cmake ../llvm -DCMAKE_BUILD_TYPE:STRING=Release
	make -j`sysctl -n hw.logicalcpu`


PS.文件很多，也很大，如果clone缓慢，请先设置代理



2.编写clang插件



	//printClsPlugin.cpp
	#include <iostream>
	#include "clang/Frontend/FrontendPluginRegistry.h"
	#include "clang/AST/AST.h"
	#include "clang/AST/ASTConsumer.h"
	#include "clang/Frontend/CompilerInstance.h"
	#include "clang/AST/RecursiveASTVisitor.h"
	using namespace clang;
	using namespace std;
	using namespace llvm;
	namespace ClangPlugin
	{
	class ClangPluginASTVisitor : public 	
	RecursiveASTVisitor<ClangPluginASTVisitor>
	{
	private:
		ASTContext *context;
	public:
		void setContext(ASTContext &context){
			this->context = &context;
	 	}
	 	bool VisitDecl(Decl *decl){
	 		if(isa<ObjCImplDecl>(decl)){
	 			ObjCImplDecl *interDecl = (ObjCImplDecl*)decl;
	 			cout<<"[KWLM]Class Implementation Found:"
	 			<<interDecl->getNameAsString()<<endl;
	    		}
	        return true;
	    }
	};
	class ClangPluginASTConsumer : public ASTConsumer
	{
	private:
		ClangPluginASTVisitor visitor;
	    void HandleTranslationUnit(ASTContext &context){
	    	visitor.setContext(context);
	    	visitor.TraverseDecl(context.getTranslationUnitDecl());
	    }
	};
	class ClangPluginASTAction : public PluginASTAction
	{
	public:
	    unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &Compiler,StringRef InFile){
	    	return unique_ptr<ClangPluginASTConsumer>(new ClangPluginASTConsumer);
	    }
	    bool ParseArgs(const CompilerInstance &CI, const std::vector<std::string>& args){
	    return true;
	    }
	};
	}
	static clang::FrontendPluginRegistry::Add
	<ClangPlugin::ClangPluginASTAction>X("ClangPlugin", 	
	"ClangPlugin");

 

 3.编译生成插件(dylib)



	clang -std=c++11 -stdlib=libc++ -L/opt/local/lib -
	L/opt/llvm/llvm_build/lib  
	-I/opt/llvm/llvm_build/tools/clang/include -
	I/opt/llvm/llvm_build/include -
	I/opt/llvm/llvm/tools/clang/include -I/opt/llvm/llvm/include -
	dynamiclib -Wl,-headerpad_max_install_names -lclang -
	lclangFrontend -lclangAST -lclangAnalysis -lclangBasic -
	lclangCodeGen -lclangDriver -lclangFrontendTool -lclangLex -
	lclangParse -lclangSema -lclangEdit -lclangSerialization -
	lclangStaticAnalyzerCheckers -lclangStaticAnalyzerCore -
	lclangStaticAnalyzerFrontend -lLLVMX86CodeGen -
	lLLVMX86AsmParser -lLLVMX86Disassembler -lLLVMExecutionEngine 
	-lLLVMAsmPrinter -lLLVMSelectionDAG -lLLVMX86AsmPrinter -
	lLLVMX86Info -lLLVMMCParser -lLLVMCodeGen -lLLVMX86Utils -
	lLLVMScalarOpts -lLLVMInstCombine -lLLVMTransformUtils -
	lLLVMAnalysis -lLLVMTarget -lLLVMCore -lLLVMMC -lLLVMSupport -
	lLLVMBitReader -lLLVMOption -lLLVMProfileData -lpthread -
	lcurses -lz -lstdc++ -fPIC -fno-common -Woverloaded-virtual -
	Wcast-qual -fno-strict-aliasing -pedantic -Wno-long-long -Wall 
	-Wno-unused-parameter -Wwrite-strings -fno-rtti -fPIC 	
	./printClsPlugin.cpp -o ClangPlugin.dylib



4.编写测试用oc文件
​	

	//ocClsDemo.m
	#import<UIKit/UIKit.h>
	@interface MyViewController : UIViewController
	@end
	
	@implementation MyViewController
	- (instancetype)init{
		if(self = [super init]){
		}
		return self;
	}
	@end


5.使用1生成的clang编译oc文件(载入3生成的ClangPlugin.dylib)



	/opt/llvm/llvm_build/bin/clang -isysroot	/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator10.0.sdk -I/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/include/c++/v1 
	-mios-version-min=8.0 -Xclang -load -Xclang 		
	~/Desktop/ClangPlugin.dylib -Xclang -add-plugin -Xclang 
	ClangPlugin -c ./ocClsDemo.m

结果如下图所示:
![clang插件运行结果](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/clang-find-user-defined-class.png)

##Xcode集成Clang插件



	要在Xcode中使用Clang插件，需要Hack Xcode.app。

[下载XcodeHacking.zip](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/others/XcodeHacking.zip)

	执行:
	sudo mv HackedClang.xcplugin `xcode-select -print-
	path`/../PlugIns/Xcode3Core.ideplugin/Contents/SharedSupport/Developer/Library/Xcode/Plug-ins
	sudo mv HackedBuildSystem.xcspec `xcode-select -print-
	path`/Platforms/iPhoneSimulator.platform/Developer/Library/Xcode/Specifications
	之后在Xcode->Target-Build Settings->Build Options->Compiler for C/C++/Objective-C选择Clang LLVM Trunk即可使得Xcode使用1生成的的Clang来编译。至于其他的命令行参数，均可以通过Xcode设置完成。
[Contact me](mailto:kang.wang1988@gmail.com)