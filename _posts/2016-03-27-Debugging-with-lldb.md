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
	

#### rewrite a .m to .cpp
	clang -rewrite-objc -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator.sdk ./main.m
#### Using clang to analyze AST(abstract syntax tree) for your code.
	