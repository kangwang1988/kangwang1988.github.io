---
layout: post
comments: true
section-type: post
title: 如何实现不越狱实现微信抢红包？
category: tech
tags: [ 'tutorial' ]
---
### Content
	1.Mach-o file format
	2.dylib create and injection
	3.Wechat analyze and hack
	4.Resign
	5.References
### Mach-o file format 
	
	Mach-o file format is the standard used to store program and library on disk in the Mac App Binary Interface(ABI). 
	
A typical Mach-o file is given below: 

![Mach-o file format](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/mach_o_segments.gif)

Segment | Description 
------------ | -------------
__PAGEZERO | The first segment of an executable file.It is located at virtual memory location 0 and has no protection rights assigned, the combination of which causes accesses to NULL, a common C programming error, to immediately crash. 
__TEXT | The segment contains `executable` code and other `read-only` data. To allow the kernel to map it directly from the executable into sharable memory, the static linker sets this segment’s virtual memory permissions to disallow writing. When the segment is mapped into memory, it can be shared among all processes interested in its contents. 
__DATA | The segment contains writable data. The static linker sets the virtual memory permissions of this segment to allow both reading and writing. Because it is writable, the `__DATA` segment of a framework or other shared library is logically copied for each process linking with the library. When memory pages such as those making up the __DATA segment are readable and writable, the kernel marks them copy-on-write; therefore when a process writes to one of these pages, that process receives its own private copy of the page.
__OBJC | The segment contains data used by the Objective-C language runtime support library.
__IMPORT | The segment contains symbol stubs and non-lazy pointers to symbols not defined in the executable. This segment is generated only for executables targeted for the IA-32 architecture.
__LINKEDIT | The segment contains raw data used by the dynamic linker, such as symbol, string, and relocation table entries.

[Find more details about Mach-o from Apple.](https://developer.apple.com/library/mac/documentation/DeveloperTools/Conceptual/MachORuntime/index.html#//apple_ref/doc/uid/20001298-89026)

With a WeChat.ipa downloaded from the jailbroken channel, we can find its inner content using MachOView:
![wechat-load-commands-in-machoview-original](https://github.com/kangwang1988/kangwang1988.github.io/raw/master/img/wechat-load-commands-in-machoview-original.png)

Those dylib 

