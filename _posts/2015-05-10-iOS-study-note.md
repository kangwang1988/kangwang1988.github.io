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
