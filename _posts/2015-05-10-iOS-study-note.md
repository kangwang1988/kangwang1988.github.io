---
layout: post
section-type: post
title: iOS study note
category: tech
tags: [ 'ios' ]
---

#### Runtime

	respondstoselector
	instancerespondstoselector
	forwardingtargetforselector
	resolveinstancemethod
	resolveclassmethod
	load

	objc_getassociatedobject/set
	objc_getClassList

	imp_implementationWithBlock

	class_getName
	class_getmethod(class,instance)/addmethod/	replacemethod
	class_copymethodlist
	class_copypropertylist/property_getname

	method_getTypeEncoding
	method_getimplementation
	method_exchange
	
#### NSURL

	NSURL
	scheme
	host
	port
	path
	parameter
	query
	fragment

#### NSLogger

	NSLog->__FILE__ __func__ __LINE__, _cmd,self
	LogData/Image->Image或者Data->Boujour
	base64 编码图片 Pros and Cons
	图片无需再次请求
	无法缓存

#### classdump

	classdump ./App.app > api.txt
	print property,method list of a objective-c app

#### dsym file

	dwarfdump --uuid dsym/app file <get a uuid>
	crashlog -> indident identifier (They two would be 	exactly the same.)
	To analyze crash logs, we have two options:
	1.Readable crash callstack.
	2.Combine the crash log with the dsym file.(dwarfdump 	command)
	--uuid & --lookup
	
#### ARC

	- fobjc-arc/fno-objc-arc single file enable/disable 	ARC
	- MRC & ARC -> Reference counting.Compiler add 	retain/release when needed.
	- Efficiency may be better than MRC.(Add release/	autorelease to realize MRC, maybe some opimization)

#### Autolayout

	- Use xib for constraints
	- Use Code <NSLayoutConstraint>
	- Use Visual Formatted Language

#### Objective C (pros and cons)
 
	Pros
	- Runtime (mini and powerful)
	- Base on C (current compile compatible and lots of library available)
	Cons
	- Garbage collection
	- Namespace
	- Operator override
	- Multiple inheritance
	- Dynamic type make it hard for compiler optimization

#### @encode

	- all data types, class,method could be expressed in ASCII encoded string.
	- No need to link it in the build phases
	- if referenced, will be copyed,otherwise do nothing
	- can add specific file.

#### Animation

	- UIView animation over Core Animation Over Core Graphics
	- UIView animation:duration delay option curve animationblock completionblock
	- CoreAnimation:CABasicAnimation、CAKeyFrameAnimation、CAAnimationGroup(duration\timingfunction\values\fromvalue\tovalue\delegate\strong)
	- CoreAnimation focus on content vs CoreGraphics focus on vector view.

#### Notification(event occuring) and KVO(value change)

	- They are both based on Observer Pattern
	- KVO is implemented by NSObject while Notification is implemented in NSNotification
	- KVO is ofter used to observe value change while notification is used for some accident.
	- [NSNotificationCenter defaultCenter] addobserver:selector:name:object
	- Object addObserver:forKeyPath:options:context:
	- It is possible to use delegates array with key of(notificationname,keypath,etc)
	- Notification and KVO are 1->multiple need to be removed(not easy to trace,third party participated)
	- KVO(string,refactor will make it not work,only one method with multiple ifs,remove)
	- Delegate is 1->1,need to be set to nil(1->1,delegate nil)

#### NSOperation Queue vs GCD

	- For task-based concurrency model
	- NSOperation Queue is OC level,GCD is C level.
	- NSOperation is internal implemented with GCD
	- If you want to simply use a block , no need for high-level abstract(cancel,number limits,dependency) use GCD, otherwise use NSOperationQueue. 		dispatch_after,dispatch_async,dispatch_once,dispatch_group,dispatch_wait,dispatch_notify
	- NSOperationQueue high-level abstract,make it 	little work for limit control,cancel,dependency;If 	you only need to use a block, no need for the 	additional advantages, use GCD.

#### @block

	Function pointer
	Have access to outer variables Function & Enviroment
	Object
	Global & Stack & Heap
	Block->Global,Stack(Using any local variable) BlockCopy(Stackblock->HeapBlock)
	- Block retain,release,copy->retaincount always be 1
	- Local auto object -> copy
	- static,global object -> address
	- OC object -> address
	- static,global,non-oc object -> won't retain
	- local object,self.xxx object -> will retain
	- to make a object to be editable in a block, use 	__block to make it like static,global
	- In MRC,__block won't retain, in ARC, __block will retain.
	- Don't release a object when it is still in use(crash) and don't retain a object when it is no use(retain cycle).Avoid retain cycle.
	
#### CoreData

	- Model
	- Context
	- Coordinator(Model ->persistence Type,URL)
	- Insert -> NSEntityDescription insertNewObjectForEntityForName:inManagedObjectContext: 	Save
	- Fetch -> NSEntityDescription entityForName:inManagedObjectContext: Request setEntity: Context Fetch Request
	- Concurrency -> Coordinator shared, Context mergeusing notification.
	
#### NSRunloop

	- Event Handling Runloop
	- InputSource & Timer
	- Timer Schedule -> add to current runloop/Default Mode
	- Other timer, addtimer formode:
	- In a thread, you have to manually start the runloop.
	- NSStream,NSURLConnection->Runloop needed; First create a request, then scheduleinrunloop:formode,start request.
	- NSRunloopCommonMode(NSRunloopDefaultMode,) 
	1.Harm->Timer(schedule->add to current runloop in defaultmode,otherwise,you may have to add it in current runloop with specific mode.)
	2.Timer & Events, 
	3.Runloop runMode:beforeDate
	4.NSURLConnection,NSStream->YES->DefaultMode,UITrackingRunloopMode will be sorry.NO->start:NO,schedule:CommonMode(Default,Tracking),start manually.
	InputSource:Timer,Selector,Port,CustomSrc.
	
#### CoreData

	- Context 上下文
	- Model 数据模型
	- Coordinator 
	- NSManagedObject
	- Storage
	
#### CoreText

	- Safari is a good web browser, but consumes a lot of memory.
	- CoreText a great rendering engine.
	- Context
	- AttributedString
	- CTFrameSettingref
	- CTFrame(CGPath、VisibleRange)
	- Draw
	- Transform
	
#### CoreImage

	- CIContext -> Options
	- CIImage -> Content
	- CIFilter ->Filter Processing.
	
#### Autoreleased LocalObject

	- init -> lock
	- dealloc -> unlock
	
#### JS call OC

	- open -> NSURLProtocol -> register/scan decide / handle can initwithrequest
	- iframe -> change src -> shouldstartloadwithrequest
	
#### Extension Communicate With App

	- App Groups
	- UserDefaults/ShareFile
	
#### Pan Move a Cell

	- Create a snapshot view
	- Pan Gesture Recognizer Delegate, move it.
	- When finished,change datasource,update UI.
	
#### Literal

	- Number
	- Array
	- Dictionary
	
#### UIButton

	- Button 排他点击 ExclusiveTouch  YES
	
#### iOS design pattern

	- 构造器 (原型、抽象工厂、工厂方法、生成器、单例)
	- 适配 （适配、桥接、外观）
	- 解耦（观察者、中介者）
	- 抽象集合 （组合、迭代器）
	- 扩展 （访问者、装饰、责任链）
	- 算法 （模板、命令、策略）
	- 性能 （代理、享元）
	- 对象状态 （备忘录）
	
#### Array PerformSelector

	- makeObjectsPerformSelector
	
#### Object ownership

	- alloc new copy mutablecopy
	
#### iOS Events

	- 触摸事件、传感器、远程控制
	- Reponse Chain: UIView->SuperView->Controller/SuperView->Window->Application(处理机制)
	- HitTest chain: Reverse.
	
#### ImageIO

	read image data from source and write it into destination.
	
#### self/super

	- they are one things.
	- however,when super appears, runtime will use objc_msgSendSuper instead of objc_msgSend.
	
#### Literal Objects

	- Immutable
	- Must be foundation objects.
	
#### About Objective C

	Avoid preprocessor defines. No type information,just replace.
	Global Constants.External and Define.
	
#### Use Enumerations

	- Use Enumerations for States,Status,Options.
	- NS_ENUM/NS_OPTIONS
	- 2^power
	- switch-default
	
#### Memory Layout

	- Class{@public NSDate *_dateOfBirth;NSString 	*firstName;NSString *_lastName;}
	- [           Class]
	- [+0         firstName]
	- [+0         secondName]
	- [+0         birthOfDate]
	
#### Property Modified

	- assign
	- strong
	- weak
	- unsafe_unretained
	- copy
	
#### Remember
	
	- @property -> data encapsulates
	- atomic,nonatomic
	- read using direct access and write using setter.(direct could be faster because there is no objc_msgSend like calls,by kvo will be bypassed,setter,debug.)
	- Read directly, and write using setter.(faster/kvo,setter,debug)
	- Equal(==,class,equalto,hash).
	- Equal must have same hash, but objects with the same hash may not be equal.
	- objc_get/set/remove associatedobjects
	- id returnvalue = objc_msgSend(someobject,@selector(messageName),parameter)
	- objc_msgSendSuper
	- resolveInstanceMethod(class_addMethod) -> will be called.
	- Message Forwarding
	- 1.Resolve Instance Method(resolve class method)
	- 2.forwardingTargetForSelector
	- 3.forwardInvocation
	- Message Forwarding happeds when a object is found not to be responding to a selector.
	- Dynamic
	- Forwarding Target
	- Full Forwarding
	- Selector Table:
	- MethodName -> IMP
	- Switch
	- Exchange
	- Method Swizzerling is often used for debug, not because it can.
	- Method swizzling.
	- class
	- {
	- isa
	- super_class
	- name
	- version
	- info 
	- instance_size
	- ivars
	- methodlist
	- caches
	- protocols
	- }