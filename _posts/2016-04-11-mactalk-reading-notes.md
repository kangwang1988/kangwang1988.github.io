---
layout: post
comments: true
section-type: post
title: 《Mactalk》读书笔记
category: tech
tags: [ 'book' ]
---
### Contents
	1.What's and why lldb?
	2.LLDB commands usage.
	3.Applications
	4.Python lldb
	5.References
### What's and why lldb?
	LLVM(Low Level Virtual Machine)是一个编译器基础架构的工程。其旨在通过提供一组带有良好定义接口可充用的库，用于编译器前端和后端的开发。
经典编译器3阶段设计:
![Class compiler three-phases](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/classic-compiler-three-phase.png)
LLVM的3阶段设计
![LLVM three-phases](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/llvm-compiler-three-phases.png)

### What's lldb?
1.	LLVM的组件包括clang(前端编译器)，libc++(llvm的C++标准库)，lldb(调试器)等。
2.	作为LLVM提供的软件调试器，lldb(LLDB debugger)虽处于早期开发当中，依旧提供了对C,C++,Objective-C,Swift语言程序的调试支持。
3.	lldb继承了GDB的优点，弥补了GDB的不足，如GUI，性能改善，插件式支持(提供C++/Python的API)和扩展。
4.	LLDB可工作于OSX，Linux，FreeBSD，Windows，支持i386，x86-64,ARM架构。从Xcode4.3开始，lldb即为默认调试器。
5.	[LLDB代码全部开源。](lldb.llvm.org/)

### Why lldb？
1.	Xcode默认调试器，功能强大。
2.	很多常用命令同gdb，从gdb转过来无不适应感。
3.	良好使用可以改进开发和调试过程。
4.	在更基础的层面理解系统和代码。
5.	分析和学习好的代码设计。
6.	分析系统可用API，开发某些定制化功能/需求。
7.	调试器本身支持插件化，可定制实现功能。

### Commands
1.	help-Show a list of all debugger commands, or give details
2.	breakpoint-A set of commands for operating on breakpoints.
3.	command- A set of commands for managing or customising the debugger commands.
4.	disassemble - Disassemble bytes in the current function, or elsewhere in the 5.	executable program as specified by the user.
6.	expression - Evaluate an expression (ObjC++ or Swift) in the current program context, using user defined variables and variables currently in scope.
7.	frame - A set of commands for operating on the current thread’s frame.
8.	gdb-remote- Connect to a remote GDB server.  If no hostname is provided,localhost is assumed.
9.	memory - A set of commands for operating on memory.
10.	process - A set of commands for operating on a process.
11.	register- A set of commands to access thread register.
12.	script - Pass an expression to the script interpreter for evaluation and return the results. Drop into the interactive interpreter if no expression is given.
13.	watchpoint - A set of commands for operating on watchpoints.
14.	image - ('target modules')  A set of commands for accessing information for one or more target modules.

#### command: breakpoint(br)
1.	set/list/enable/disable/delete
2.	breakpoint set -S "length" 
3.	breakpoint set -F "[UIViewController viewDidLoad]”
4.	breakpoint set -r “initWithTitle:*”
5.	breakpoint set -F "[NKHomeViewController viewWillAppear:]" -c "shownTimes == 3”
6.	breakpoint command add 3

#### command: command
![Command Usage](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/lldb-command-command.png)

#### command: disassemble
![Command:disassemble 1](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/lldb-command-disassemble.png)
![Command:disassemble 2](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/lldb-command-disassemble-2.png)

#### command:expression
1.	p:an abbreviation for 'expression —'
2.	po:an abbreviation for 'expression -O  -- ‘
3.	print:an abbreviation for 'expression --'

![Command:expression 1](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/lldb-command-expression.png)
![Command:expression 2](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/lldb-command-expression2.png)
![Command:expression 3](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/lldb-command-expression3.png)

#### command:frame (栈帧)
![Command:frame](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/lldb-command-frame.png)

#### command:gdb-remote(远程调试协议)
	process connect connect://10.0.77.165:1234
	
#### command:memory(malloc_info)
1.	find/history/read/write
![Command:memory](https://github.com/kangwang1988/kangwang1988.github.io/raw/master/img/lldb-command-memory.png)

#### command:process
	attach/detach,
	continue/interrupt,
	load/unload,
	launch/signal/kill
	
#### command:register
![Command:register](https://github.com/kangwang1988/kangwang1988.github.io/raw/master/img/lldb-command-register.gif)
![Command:register2](https://github.com/kangwang1988/kangwang1988.github.io/raw/master/img/lldb-command-arm-arch.png)

#### command:script
![Command:script](https://github.com/kangwang1988/kangwang1988.github.io/raw/master/img/lldb-command-script.png)

#### command:watchpoint(wa)
	set/list/enable/disable/delete
	
#### command:image(镜像)
	add/dump/list/load/lookup
	
#### command:thread (Flow Control)
![Command:flow control](https://github.com/kangwang1988/kangwang1988.github.io/raw/master/img/lldb-command-thread.gif)

### Application: (UI Related)
	打印层次:
	po [aView recursiveDescription]
	更改UI:
	expression [self.tableView setBackgroundColor:[UIColor redColor]
	expression (void)[CATransaction flush]
	Create/Init/push a VC is also available.
	
#### Application: (Where is the alert from?)
![Application:Alert from](https://github.com/kangwang1988/kangwang1988.github.io/raw/master/img/lldb-application-where-is-alert-from.png)

	Approach 1:
	Method Swizzle(hook).

![Application:Alert from2](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/rage-comic-proud.png)

	Approach 2:	
	breakpoint set -r “initWithTitle:*”
![Application:Alert from3](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/lldb-application-br-initwithtitle.png)

![Application:Alert from4](https://github.com/kangwang1988/kangwang1988.github.io/raw/master/img/rage-comic-cry.png)

#### Application: (Who change the data?)

![Application:Data changed](https://github.com/kangwang1988/kangwang1988.github.io/raw/master/img/lldb-application-watchpoint-who-changed-the-data.png)

	Approach 1:
	Override Setter
	
	Approach 2:
	KVO
	
![Application:Data changed2](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/rage-comic-proud.png)

	Approach3:
	watchpoint set expression self.dataModel->_dataSource
![Application:Data changed3](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/lldb-application-watchpoint-who-changed-the-data2.png)

![Application:Data changed4](https://github.com/kangwang1988/kangwang1988.github.io/raw/master/img/rage-comic-cry.png)

#### Application:(debugserver)

	debugserver是用于辅助gdb或者lldb远程调试的命令行APP。当iOS设备用于开发时其会被安装。Xcode	调试时，会触发此进程用于远程调试。
	越狱设备调试时，首先拷贝debugserver到越狱机，然后使用debugserver启动需要调试的APP，	lldb(gdb)使用gdb-remote协议，在debugserver的协助下，实现调试功能。
	
#### Application:(远程调试)
![Application:remote debug](https://github.com/kangwang1988/kangwang1988.github.io/raw/master/img/lldb-application-remote-debug.png)

![Application:remote debug2](https://github.com/kangwang1988/kangwang1988.github.io/raw/master/img/lldb-application-remote-debug2.png)

![Application:remote debug3](https://github.com/kangwang1988/kangwang1988.github.io/raw/master/img/lldb-application-remote-debug3.png)

![Application:remote debug4](https://github.com/kangwang1988/kangwang1988.github.io/raw/master/img/lldb-application-remote-debug4.png)

#### Application:(App Install Progress)
![Application:App Install Progress](https://github.com/kangwang1988/kangwang1988.github.io/raw/master/img/lldb-application-app-install-progress.png)

![Application:App Install Progress2](https://github.com/kangwang1988/kangwang1988.github.io/raw/master/img/lldb-application-app-install-progress2.png)

![Application:App Install Progress3](https://github.com/kangwang1988/kangwang1988.github.io/raw/master/img/lldb-application-app-install-progress3.png)

![Application:App Install Progress4](https://github.com/kangwang1988/kangwang1988.github.io/raw/master/img/lldb-application-app-install-progress4.png)

#### Application:(Xcode Tweak)
![Application:Xcode Tweak](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/lldb-applcation-xcode-tweak.png)

![Application:Xcode Tweak2](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/lldb-applcation-xcode-tweak2.png)

#### Application: (How WeChat show web progress?)
![Application:WeChat Web Progress](https://github.com/kangwang1988/kangwang1988.github.io/raw/master/img/lldb-application-how-wechat-show-web-progress.png)

![Application:WeChat Web Progress2](https://github.com/kangwang1988/kangwang1988.github.io/raw/master/img/lldb-application-how-wechat-show-web-progress2.png)

![Application:WeChat Web Progress3](https://github.com/kangwang1988/kangwang1988.github.io/raw/master/img/lldb-application-how-wechat-show-web-progress3.png)

### Application : (Analyse/Debug a third-party app)
	
	Mac
	target create /bin/ls
	breakpoint set --name malloc
	process launch /bin/ls
	或process attach —pid 123/—name Safari
	iOS
	远程调试：
	ssh(越狱)
	debugserver(ios)
	lldb(mac)

### Python plugin for lldb
	代码:
	Write Python module with a command function like:
	def <function>(debugger, command, result, internal_dict)	
	使用:
	Import module into LLDB(~/.lldbinit)
	(Bind Python function to command)
![Python Plugin](https://github.com/kangwang1988/kangwang1988.github.io/raw/master/img/lldb-python-plugin-for-lld.png)

	SBDebugger:(SB is abbr for Scripting bridge)
		The command interpreter,Always be one.
	SBTarget:
		Represents the target program under debugger.
	SBProcess:
		Contains the process of the selected target.
	SBThread:
    	Contains the process of the currently select thread.
	SBFrame:
    	Contains the selected frame.
	SBValue:
		An object that encapsulates data objects.

### References
[LLDB wiki1](https://en.wikipedia.org/wiki/LLDB_%28debugger%29)	
[LLDB wiki2](http://lldb.llvm.org/lldb-gdb.html)

[ARM architecture](http://infocenter.arm.com/help/topic/com.arm.doc.ihi0042f/IHI0042F_aapcs.pdf)

[LLDB python reference](http://lldb.llvm.org/python-reference.html)

[LLDB tutorials in Raywenderlich](https://www.raywenderlich.com/tag/lldb)

[Facebook’s chisel for lldb](https://github.com/facebook/chisel)

[Wiki for debugserver](http://iphonedevwiki.net/index.php/Debugserver)

[Dancing with lldb in objc.io](https://www.objc.io/issues/19-debugging/lldb-debugging)


### End
