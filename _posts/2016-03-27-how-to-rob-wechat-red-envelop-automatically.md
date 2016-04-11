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
### Mach-O file format 
	
	Mach-o file format is the standard used to store program and library on disk in the Mac App Binary Interface(ABI). 
	

![Mach-o file format](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/mach_o_segments.gif)

Codes below specifying the header/load commands data structure can be found in the <mach-o/loader.h> file.

	/*
 	* The 32-bit mach header appears at the very beginning of the object file for
 	* 32-bit architectures.
 	*/
		struct mach_header {
		uint32_t	magic;		/* mach magic number identifier */
		cpu_type_t	cputype;	/* cpu specifier */
		cpu_subtype_t	cpusubtype;	/* machine specifier */
		uint32_t	filetype;	/* type of file */
		uint32_t	ncmds;		/* number of load commands */
		uint32_t	sizeofcmds;	/* the size of all the load commands */
		uint32_t	flags;		/* flags */
		};

	/* Constant for the magic field of the mach_header (32-bit architectures) */
	#define	MH_MAGIC	0xfeedface	/* the mach magic number */
	#define MH_CIGAM	0xcefaedfe	/* NXSwapInt(MH_MAGIC) */

	/*
 	 * The 64-bit mach header appears at the very beginning of object files for
 	 * 64-bit architectures.
	 */
		struct mach_header_64 {
		uint32_t	magic;		/* mach magic number identifier */
		cpu_type_t	cputype;	/* cpu specifier */
		cpu_subtype_t	cpusubtype;	/* machine specifier */
		uint32_t	filetype;	/* type of file */
		uint32_t	ncmds;		/* number of load commands */
		uint32_t	sizeofcmds;	/* the size of all the load commands */
		uint32_t	flags;		/* flags */
		uint32_t	reserved;	/* reserved */
		};

	/*
 	 * The segment load command indicates that a part of this file is to be
 	 * mapped into the task's address space.  The size of this segment in memory,
 	 * vmsize, maybe equal to or larger than the amount to map from this file,
 	 * filesize.  The file is mapped starting at fileoff to the beginning of
 	 * the segment in memory, vmaddr.  The rest of the memory of the segment,
 	 * if any, is allocated zero fill on demand.  The segment's maximum virtual
 	 * memory protection and initial virtual memory protection are specified
 	 * by the maxprot and initprot fields.  If the segment has sections then the
 	 * section structures directly follow the segment command and their size is
 	 * reflected in cmdsize.
 	 */
		struct segment_command { /* for 32-bit architectures */
			uint32_t	cmd;		/* LC_SEGMENT */
			uint32_t	cmdsize;	/* includes sizeof section structs */
			char		segname[16];	/* segment name */
			uint32_t	vmaddr;		/* memory address of this segment */
			uint32_t	vmsize;		/* memory size of this segment */
			uint32_t	fileoff;	/* file offset of this segment */
			uint32_t	filesize;	/* amount to map from the file */
			vm_prot_t	maxprot;	/* maximum VM protection */
			vm_prot_t	initprot;	/* initial VM protection */
			uint32_t	nsects;		/* number of sections in segment */
			uint32_t	flags;		/* flags */
		};

	/*
	 * The 64-bit segment load command indicates that a part of this file is to be
	 * mapped into a 64-bit task's address space.  If the 64-bit segment has
 	* sections then section_64 structures directly follow the 64-bit segment
 	* command and their size is reflected in cmdsize.
 	*/
		struct segment_command_64 { /* for 64-bit architectures */
		uint32_t	cmd;		/* LC_SEGMENT_64 */
		uint32_t	cmdsize;	/* includes sizeof section_64 structs */
		char		segname[16];	/* segment name */
		uint64_t	vmaddr;		/* memory address of this segment */
		uint64_t	vmsize;		/* memory size of this segment */
		uint64_t	fileoff;	/* file offset of this segment */
		uint64_t	filesize;	/* amount to map from the file */
		vm_prot_t	maxprot;	/* maximum VM protection */
		vm_prot_t	initprot;	/* initial VM protection */
		uint32_t	nsects;		/* number of sections in segment */
		uint32_t	flags;		/* flags */
		};

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

It's clear that the dylib link info is stored in the Load Commands(LC_LOAD_DYLIB) part. Henceforth, if we create a dylib and inject it into the binary by modifying the load commands, we might do something we want in WeChat.


### dylib create and injection.
	
1.Write your source code.

	//test.m
	__attribute__((constructor))
	static void dylibRuntimeInjection() {    		dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(1.5 * 		NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
        UIAlertView *alertView = [[UIAlertView alloc] initWithTitle:@"Hacked" 		message:@"Plugin for WeChat injected" delegate:nil 		cancelButtonTitle:@"哈哈" otherButtonTitles:nil, nil];
   		[alertView show];
  		});
	}

ps. The constructor attribute causes the function to be called automatically before execution enters main (). Similarly, the destructor attribute causes the function to be called automatically after main () has completed or exit () has been called. Functions with these attributes are useful for initializing data that will be used implicitly during the execution of the program.__attribute__((constructor)) is not Standard C++. It is GCC's extension.

2.Compile the .m file(s) into dylib using clang.

	clang -arch armv7 -arch arm64 -isysroot $(xcodebuild -sdk iphoneos -version Path) -shared test.m -framework Foundation -framework UIKit -o test.dylib
		
3.Inject the dylib into the binary file.

	optool install -c load -p "@executable_path/test.dylib" -t ./WeChat.app/WeChat
	cp ../test.dylib ./WeChat.app/
	
p.s optool is a Mac command line app which helps you handle the Mach-O file.You may find its source code in Github.[Clone optool](https://github.com/alexzielenski/optool)
![wechat-load-commands-in-machoview-injected](https://github.com/kangwang1988/kangwang1988.github.io/raw/master/img/wechat-load-commands-in-machoview-injected.png)

4.Resign it with your certificate.
	After injection, you may wanna to install the ipa into your device to have a try. However, as the original one is signed with a certificate which you don't have, the app will fail when launching as the signing identity doesn't match.(Remember the dylib you created?) So, you may want to resign it with your certificate, if possible, the bundle id might be also altered.
	
	codesign -fs "$certname" "Payload/${appname}.app/${pluginname}.dylib"
	codesign -fs "$certname" --no-strict --entitlements="$entitlements" "${tempDir}/Payload/${appname}.app"
	
5.Install it using itools.
	![wechat-plugin-injected](https://github.com/kangwang1988/kangwang1988.github.io/raw/master/img/wechat-plugin-injected.png)

### Rob red envelop plugin

1.Design
a.	Find when a red envelop is available and choose a moment to open it.
b.	Find which class and selector you would like to hook.
c.	Consider cases like there are several envelops avaliable, provide interfaces for user to enable/disable this feature.
d.	Write, compile, inject, resign, install, test and redo the former procedures until success.