---
layout: post
section-type: post
title: Clang usage tips
category: tech
tags: [ 'technique' ]
---
### What is clang?
	
	a C language family frontend for LLVM
	The goal of the Clang project is to create a new C, C++, Objective C and Objective C++ front-end for the LLVM compiler. You can get and build the source today.

### rewrite a .m to .cpp

	clang -rewrite-objc -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator.sdk ./main.m

### Using clang to analyze AST(abstract syntax tree) for your code.
	