# LLDB
##Contents
	1.What's and why lldb?
	2.LLDB commands usage.
	3.Applications
	4.Python lldb
	5.References
## What's and why lldb?
	LLVM(Low Level Virtual Machine)是一个编译器基础架构的工程。其旨在通过提供一组带有良好定义接口可充用的库，用于编译器前端和后端的开发。
经典编译器3阶段设计:
![Class compiler three-phases](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/_images/classic-compiler-three-phase.png)
LLVM的3阶段设计
![LLVM three-phases](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/_images/llvm-compiler-three-phases.png)

## What's lldb?
1.	LLVM的组件包括clang(前端编译器)，libc++(llvm的C++标准库)，lldb(调试器)等。
2.	作为LLVM提供的软件调试器，lldb(LLDB debugger)虽处于早期开发当中，依旧提供了对C,C++,Objective-C,Swift语言程序的调试支持。
3.	lldb继承了GDB的优点，弥补了GDB的不足，如GUI，性能改善，插件式支持(提供C++/Python的API)和扩展。
4.	LLDB可工作于OSX，Linux，FreeBSD，Windows，支持i386，x86-64,ARM架构。从Xcode4.3开始，lldb即为默认调试器。
5.	[LLDB代码全部开源。](lldb.llvm.org/)

## Why lldb？
1.	Xcode默认调试器，功能强大。
2.	很多常用命令同gdb，从gdb转过来无不适应感。
3.	良好使用可以改进开发和调试过程。
4.	在更基础的层面理解系统和代码。
5.	分析和学习好的代码设计。
6.	分析系统可用API，开发某些定制化功能/需求。
7.	调试器本身支持插件化，可定制实现功能。

## Commands
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

### command: breakpoint(br)
1.	set/list/enable/disable/delete
2.	breakpoint set -S "length" 
3.	breakpoint set -F "[UIViewController viewDidLoad]”
4.	breakpoint set -r “initWithTitle:*”
5.	breakpoint set -F "[NKHomeViewController viewWillAppear:]" -c "shownTimes == 3”
6.	breakpoint command add 3

### command: command
![Command Usage](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/_images/lldb-command-command.png)

### command: disassemble
![Command:disassemble 1](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/_images/lldb-command-disassemble.png)
![Command:disassemble 2](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/_images/lldb-command-disassemble-2.png)

### command:expression
1.	p:an abbreviation for 'expression —'
2.	po:an abbreviation for 'expression -O  -- ‘
3.	print:an abbreviation for 'expression --'

![Command:expression 1](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/_images/lldb-command-expression.png)
![Command:expression 2](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/_images/lldb-command-expression2.png)
![Command:expression 3](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/_images/lldb-command-expression3.png)

### command:frame (栈帧)
![Command:frame](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/_images/lldb-command-frame.png)

### command:gdb-remote(远程调试协议)
	process connect connect://10.0.77.165:1234
	
### command:memory(malloc_info)
1.	find/history/read/write
![Command:memory](https://github.com/kangwang1988/kangwang1988.github.io/raw/master/_images/lldb-command-memory.png)

### command:process
	attach/detach,
	continue/interrupt,
	load/unload,
	launch/signal/kill
	
### command:register
![Command:register](https://github.com/kangwang1988/kangwang1988.github.io/raw/master/_images/lldb-command-register.gif)
![Command:register2](https://github.com/kangwang1988/kangwang1988.github.io/raw/master/_images/lldb-command-arm-arch.png)

### command:script
![Command:script](https://github.com/kangwang1988/kangwang1988.github.io/raw/master/_images/lldb-command-script.png)

### command:watchpoint(wa)
	set/list/enable/disable/delete
	
### command:image(镜像)
	add/dump/list/load/lookup
	
### command:thread (Flow Control)
![Command:flow control](https://github.com/kangwang1988/kangwang1988.github.io/raw/master/_images/lldb-command-thread.gif)

### Application: (UI Related)
	打印层次:
	po [aView recursiveDescription]
	更改UI:
	expression [self.tableView setBackgroundColor:[UIColor redColor]
	expression (void)[CATransaction flush]
	Create/Init/push a VC is also available.
	
### Application: (Where is the alert from?)
![Application:Alert from](https://github.com/kangwang1988/kangwang1988.github.io/raw/master/_images/lldb-application-where-is-alert-from.png)

	Approach 1:
	Method Swizzle(hook).

![Application:Alert from2]()
	