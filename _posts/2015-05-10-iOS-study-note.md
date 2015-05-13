---
layout: post
title: iOS study note
---

@HangZhou
<br>
================ Runtime ================<br>
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
class_getmethod(class,instance)/addmethod/replacemethod
class_copymethodlist
class_copypropertylist/property_getname

method_getTypeEncoding
method_getimplementation
method_exchange
==================================<br>
================ NSURL ================<br>
NSURL
scheme
host
port
path
parameter
query
fragment
==================================<br>
================ NSLogger ================<br>
NSLog->__FILE__ __func__ __LINE__, _cmd,self
LogData/Image->Image或者Data->Boujour
base64 编码图片 Pros and Cons
图片无需再次请求
无法缓存
==================================<br>
================ class-dump ================<br>
classdump ./App.app > api.txt
print property,method list of a objective-c app
==================================<br>
================ dsym file  ================<br>
dwarfdump --uuid dsym/app file <get a uuid>
crashlog -> indident identifier (They two would be exactly the same.)
To analyze crash logs, we have two options:
1.Readable crash callstack.
2.Combine the crash log with the dsym file.(dwarfdump command)
--uuid & --lookup
==================================<br>
================ ARC  ================<br>
- fobjc-arc/fno-objc-arc single file enable/disable ARC
- MRC & ARC -> Reference counting.Compiler add retain/release when needed.
- Efficiency may be better than MRC.(Add release/autorelease to realize MRC, maybe some opimization)
==================================<br>
================ Autolayout  ================<br>
- Use xib for constraints
- Use Code <NSLayoutConstraint>
- Use Visual Formatted Language
==================================<br>
================ Objective C (pros and cons)  ================<br>
- Pros
- Runtime (mini and powerful)
- Base on C (current compile compatible and lots of library available)
- Cons
- Garbage collection
- Namespace
- Operator override
- Multiple inheritance
- Dynamic type make it hard for compiler optimization
==================================<br>
================ @encode  ================<br>
- all data types, class,method could be expressed in ASCII encoded string.
==================================<br>
================ @import  ================<br>
- No need to link it in the build phases
- if referenced, will be copyed,otherwise do nothing
- can add specific file.
==================================<br>
================ Animation  ================<br>
- UIView animation over Core Animation Over Core Graphics
- UIView animation:duration delay option curve animationblock completionblock
- CoreAnimation:CABasicAnimation、CAKeyFrameAnimation、CAAnimationGroup(duration\timingfunction\values\fromvalue\tovalue\delegate\strong)
- CoreAnimation focus on content vs CoreGraphics focus on vector view.
==================================<br>
================ Notification(event occuring) and KVO(value change)  ================<br>
- They are both based on Observer Pattern
- KVO is implemented by NSObject while Notification is implemented in NSNotification
- KVO is ofter used to observe value change while notification is used for some accident.
- [NSNotificationCenter defaultCenter] addobserver:selector:name:object
- Object addObserver:forKeyPath:options:context:
- It is possible to use delegates array with key of(notificationname,keypath,etc)
- Notification and KVO are 1->multiple need to be removed(not easy to trace,third party participated)
- KVO(string,refactor will make it not work,only one method with multiple ifs,remove)
- Delegate is 1->1,need to be set to nil(1->1,delegate nil)
==================================<br>
================ NSOperation Queue vs GCD  ================<br>
- For task-based concurrency model
- NSOperation Queue is OC level,GCD is C level.
- NSOperation is internal implemented with GCD
- If you want to simply use a block , no need for high-level abstract(cancel,number limits,dependency) use GCD, otherwise use NSOperationQueue.
- dispatch_after,dispatch_async,dispatch_once,dispatch_group,dispatch_wait,dispatch_notify
- NSOperationQueue high-level abstract,make it little work for limit control,cancel,dependency;If you only need to use a block, no need for the additional advantages, use GCD.
==================================<br>
================ @block  ================<br>
- Function pointer
- Have access to outer variables Function & Enviroment
- Object
- Global & Stack & Heap
- Block->Global,Stack(Using any local variable) BlockCopy(Stackblock->HeapBlock)
- Block retain,release,copy->retaincount always be 1
- Local auto object -> copy
- static,global object -> address
- OC object -> address
- static,global,non-oc object -> won't retain
- local object,self.xxx object -> will retain
- to make a object to be editable in a block, use __block to make it like static,global
- In MRC,__block won't retain, in ARC, __block will retain.
- Don't release a object when it is still in use(crash) and don't retain a object when it is no use(retain cycle).Avoid retain cycle.
==================================<br>
================ CoreData  ================<br>
- Model
- Context
- Coordinator(Model ->persistence Type,URL)
- Insert -> NSEntityDescription insertNewObjectForEntityForName:inManagedObjectContext: Save
- Fetch -> NSEntityDescription entityForName:inManagedObjectContext: Request setEntity: Context Fetch Request
- Concurrency -> Coordinator shared, Context mergeusing notification.
==================================<br>
================ NSRunloop  ================<br>
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
4.NSURLConnection,NSStream->
YES->DefaultMode,UITrackingRunloopMode will be sorry.
NO->start:NO,schedule:CommonMode(Default,Tracking),start manually.
InputSource:Timer,Selector,Port,CustomSrc.
==================================<br>
================ CoreData  ================<br>
- Context 上下文
- Model 数据模型
- Coordinator 
- NSManagedObject
- Storage
==================================<br>
================ CoreText  ================<br>
- Safari is a good web browser, but consumes a lot of memory.
- CoreText a great rendering engine.
==================================<br>
================ CoreImage  ================<br>
- CIContext -> Options
- CIImage -> Content
- CIFilter ->Filter Processing.
==================================<br>
================ Autoreleased LocalObject  ================<br>
- init -> lock
- dealloc -> unlock
==================================<br>
================ JS call OC  ================<br>
- open -> NSURLProtocol -> register/scan decide / handle can initwithrequest
- iframe -> change src -> shouldstartloadwithrequest
==================================<br>
================ Extension Communicate With App  ================<br>
- App Groups
- UserDefaults/ShareFile
==================================<br>
================ Pan Move a Cell  ================<br>
- Create a snapshot view
- Pan Gesture Recognizer Delegate, move it.
- When finished,change datasource,update UI.
==================================<br>
================ Core Bluetooth  ================<br>
- 
==================================<br>
================ Literal  ================<br>
- Number
- Array
- Dictionary
==================================<br>
================ UIButton  ================<br>
- Button 排他点击 ExclusiveTouch  YES
==================================<br>
================ iOS design pattern  ================<br>
- 构造器 (原型、抽象工厂、工厂方法、生成器、单例)
- 适配 （适配、桥接、外观）
- 解耦（观察者、中介者）
- 抽象集合 （组合、迭代器）
- 扩展 （访问者、装饰、责任链）
- 算法 （模板、命令、策略）
- 性能 （代理、享元）
- 对象状态 （备忘录）
==================================<br>
================ Array PerformSelector  ================<br>
- makeObjectsPerformSelector
==================================<br>
================ Object ownership  ================<br>
- alloc new copy mutablecopy
==================================<br>
================ iOS Events  ================<br>
- 触摸事件、传感器、远程控制
- Reponse Chain: UIView->SuperView->Controller/SuperView->Window->Application(处理机制)
- HitTest chain: Reverse.
==================================<br>
