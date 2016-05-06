---
layout: post
comments: true
section-type: post
title: 性能调试器
category: tech
tags: [ 'tutorial' ]
---

### Content
	1.Performance Mesurement.
	2.Dtrace in Mac OS X
	3.Decompile & Analyze an APK
	4.Write a script to simulate it
	5.Summary
	
### Dtrace in Mac OS X
	
Hello World
	dtrace -n 'BEGIN { trace("Hello World!"); }'
	
Tracing Who Opened What
	dtrace -n 'syscall::open:entry { printf("%s %s",execname,copyinstr(arg0));} '
Tracing fork() and exec()
	dtrace -n 'syscall::fork*: {trace(pid);}'
	dtrace -n 'syscall::exec*: {trace(execname);}'
Counting System Calls by a Named process
	dtrace -n 'syscall:::entry /execname=="CLPDemo"/' {@[probefunc]=count();}
Profiling Process Names:
	dtrace -n 'profile-997 {@[execname]=count();} tick-1s {printa(@);trunc(@);}'
	