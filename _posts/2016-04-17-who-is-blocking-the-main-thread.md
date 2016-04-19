---
layout: post
comments: true
section-type: post
title: iOS UI 卡顿监测
category: tech
tags: [ 'tutorial' ]
---
### Contents

1.	Runloop
2.	Thread frames retrieve.
3.	Mainthread timeout detection
4.	Address symbolicating 

### Runloop
![Runloop](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/runloop.jpg)

	Runloop modes:
		NSDefaultRunLoopMode、UITrackingRunLoopMode、NSRunLoopCommonModes
		
### Thread  frames retrieve at runtime

Darwin kernel:

![Darwin](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/darwin.png)
	
	Mach:Micro kernel, very limited basic services like CPU scheduling, IPC etc.
	BSD: A ring surrounding Mach layer, providing process management, file system,network,etc.
	IOKit: A object oriented framework provided by device driver.

Get threads of current process:

	task_threads:Return the target task's list of threads.
	kern_return_t task_threads(task_t task,thread_act_port_array_t thread_list,					  mach_msg_type_number_t *thread_count);
	
Get thread state of certain thread:		
	
	thread_get_state:Return the execution state (for example, the machine registers) for target thread.
	kern_return_t thread_get_state(thread_act_t target_thread,
									thread_state_flavor_t flavor,
                 					thread_state_t old_state,
                 					mach_msg_type_number_t old_state_count);
                 					
    ARM_THREAD_STATE/ARM_VFP_STATE/ARM_EXCEPTION_STATE
	
Read pc from registers.
	
	_STRUCT_ARM_THREAD_STATE
	{
		__uint32_t	__r[13];	/* General purpose register r0-r12 */
		__uint32_t	__sp;		/* Stack pointer r13 */
		__uint32_t	__lr;		/* Link register r14 */
		__uint32_t	__pc;		/* Program counter r15 */
		__uint32_t	__cpsr;		/* Current program status register */
	};
	
Traverse all frames:
![Darwin](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/arm_stack.jpg)

	R7:frame pointer
	1.Get old R7 and LR from R7.
	2.Read old R7 as new R7.
	3.Goto 1 until old R7 is NULL.
	
### Mainthread timeout detection

	1.Register a observer for runloop wakeup/sleep with CFRunLoopAddObserver.
	
	CF_EXPORT void CFRunLoopAddObserver(CFRunLoopRef rl, CFRunLoopObserverRef 										 observer, CFStringRef mode);
	
	2.When the observer is called, record the current timestamp(ts) by distinguishing the CFRunLoopActivity.
	
	kCFRunLoopBeforeWaiting:Inside the event processing loop before the run loop sleeps, waiting for a source or timer to fire.
	kCFRunLoopAfterWaiting:Inside the event processing loop after the run loop wakes up, but before processing the event that woke it up. This activity occurs only if the run loop did in fact go to sleep during the current loop.
	
	3.Do time checking timely in another thread(which started with step 1).When the timeinterval between now and the recorded start time is over certain threshold, record the callback trees in each thread.
	

### Address symbolicating

	When a mainthread timeout is detected, you will get a callback tree in each threads.
	An example is given as below:
![Darwin](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/stuckbacktree.png)

	The frame is followed by an offset address(stackaddress-loadaddress).When you want to symbolicate it, you should get the slide first.
	otool -arch armv7 -l KWToolKit.app/KWToolKit | grep -B 3 -A 8 -m 2 "__TEXT"
![Darwin](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/slide.png)

	The symbol address would be:
		symbol address = slide+offset 
	
	Symbolicate the symbol using:
	dwarfdump --arch ARCHITECTURE PATH-TO-YOUR-BINARY --lookup symbol-address
	
[Check out the demo probject at github.](https://github.com/kangwang1988/KWToolKit)