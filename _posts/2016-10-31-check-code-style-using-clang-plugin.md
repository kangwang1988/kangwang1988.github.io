---
layout: post
comments: true
section-type: post
title: Clang技术分享系列二:代码风格检查(A clang plugin approach)
category: tech
tags: [ 'clang' ]

---
## 背景

   一致的代码风格检查已经是老生常谈了，统一规范的代码风格不仅可提高代码的可读性，可维护性，减少问题的发生，阅读体验也是蛮不错的。
   在系列一介绍了Clang插件的原理后，本文将介绍如何用Clang插件完成代码风格的检查及修改提示。

## 良好的代码风格长什么样？

    网上已经有很多总结，比如类前缀，驼峰命名，函数体不过长，语义清晰，良好的格式和缩进，init/dealloc不发送消息，NSString使用copy，delegate若引用等等，这里不再赘述，本文主要参考Objective-C-Coding-Guidelines-In-Chinese一文。

[Objective-C-Coding-Guidelines-In-Chinese](https://github.com/QianKaiLu/Objective-C-Coding-Guidelines-In-Chinese)

## 使用Clang检查代码风格

   Clang适合于解决给定格式的情形，对于清晰的命名等语义相关，语法糖(dot-syntax在Clang输入前已被修改)，未知类型(如面对一个String并不知道是不是用于通知的名称)并不适用。
​    
   下面就介绍若干常见的可用于Clang分析的Case及处理方式:
​    
	1.类命名:(假设需要前缀为XX，不能包含_)
	@interface user_Model : NSObject;
	@end
	所有的声明分析需要重载
	bool RecursiveASTVisitor::VisitDecl(Decl *decl);
	表达式分析则需要重载
	bool RecursiveASTVisitor::VisitStmt(Stmt *stmt);
	其中当decl:ObjCInterfaceDecl代表类声明,decl:ObjCImplDecl代表类定义，参考此两个类的方法即可完成名称的判断。
	
	2.ivar以下划线开头
	@interface user_Model : NSObject{
	NSString *ivar2;
	}
	当VisitDecl(Decl *decl)中的decl为ObjCIvarDecl代表实例变量声明，参考其方法即可分析变量名是否以下划线开头
	
	3.NSString使用copy属性
	@interface user_Model : NSObject
	@property (nonatomic,strong) NSString *name;
	@end
	当VisitDecl(Decl *decl);中的decl为ObjCPropertyDecl时代表属性，其提供了查询Attribute的方法，返回的ObjCPropertyDecl::PropertyAttributeKind可用于检查是否是copy。
	
	4.使用内置的数据类型NSInteger而不是int
	@interface user_Model : NSObject
	@property (assign,nonatomic) int age;
	@end
	当VisitDecl(Decl *decl);中的decl为ObjCPropertyDecl时代表属性，其提供了查询type的方法，即此例中的int，可用于提醒用户使用NSInteger而不是int.
	
	5.函数声明多个参数时，分行展示且冒号对齐
	@interface user_Model : NSObject
	- (instancetype)initWithMAC:(NSString *)mac AzIp:(NSString *)az_ip AzDns:(NSString *)az_dns Token:(NSString *)token Email:(NSString *)email;
	当VisitDecl(Decl *decl);中的decl为ObjCMethodDecl时代表方法，当其为声明param_size接口返回的参数数目大于参数限制时，获取其名称起始位置中间的源代码，对其进行format(分行显示且冒号对齐),判断如果format之后的类型和先前不一致，提示用户修改。
	
	6.函数变量名长度不得超过指定字符数
	@interface user_Model : NSObject
	- (void)varNameTest:(NSInteger)numberOfPeopleOnTheUsOlympicTeam;
	@end
	同5，当中的decl为ObjCMethodDecl时，遍历其params,每一个将是一个ParmVarDecl类型代表参数的变量，其getNameAsString接口可用于判断变量名长度并当超过限制时提示用户修改。
	
	7.方法代码行数不超过限制
	@implementation user_Model
	- (void)methodLenTest:(long long)nSize{
	........}
	@end
	同5，当中的decl为ObjCMethodDecl且hasBody时，getBody()获取其函数体Stmt，再利用SourceManager获取其body的源代码(string类型)，这样就可以判断代码行数是不是超过限制了。
	
	8.函数参数使用内置数据类型
	@interface user_Model
	- (void)writeVideoFrameWithData:(NSData *)frameData timeStamp:(int)timeStamp;
	@end
	同6，当中的decl为ObjCMethodDecl时，遍历其params,每一个将是一个ParmVarDecl类型代表参数的变量，其getType()接口获取到的类型转成string即可判断参数类型是否为int,如果是提醒用户。
	
	9.枚举使用NS_ENUM
	typedef enum {
	CStyleEnumTypeDefault,
	}CStyleEnumType;
	当VisitDecl(Decl *decl);中的decl为EnumDecl时代表枚举定义，通过void Decl::print(raw_ostream &Out, unsigned Indentation = 0,bool PrintInstantiation = false) const;此方法可以获取到原始代码，通过判断是否包含以typedef enum/enum开头判断是否是enum，如果是提醒用户使用NS_ENUM/NS_OPTIONS。
	
	10.delegate为weak
	@interface XXViewController : UIViewController
	@property(nonatomic,strong) id<XXViewControllerDelegate> delegate;
	@end
	同3，当VisitDecl(Decl *decl);中的decl为ObjCPropertyDecl时代表属性，其getType()转为String获取到的类型里如果包含'<'和'>'则认为是delegate，同3也可判断出是否是weak。
	
	11.delegate书写格式形如id<XXXDelegate> delegate
	@interface XXViewController : UIViewController
	@property(nonatomic,weak) id< UITextFieldDelegate>  tfDelegate;
	@end
	参考10处理delegate，并结合5对于ObjCMethodDecl名称的处理可以判断是否需要提示用户修改格式。
	
	12.属性使用nonatomic
	@interface XXViewController : UIViewController
	@property (copy) NSString *hint;
	@end
	参考3可以判断属性是否为atomic,如果是提醒用户使用nonatomic。
	
	13.消息发送多参数时，分行展示且冒号对齐
		[model initWithMAC:nil AzIp:nil AzDns:nil Token:nil Email:nil];
	当VisitStmt(Stmt *stmt)中的stmt为ObjCMessageExpr时，代表OC消息发送，其中model为receiver，initWithMac:AZip:AzDns:Token:Email:为sel.参考5，可以获取消息发送的源代码并format作比较，如果不一样则提示用户修改。
	
	14.不在init/alloc中向对象发送消息
	@implementation XXViewController
	- (instancetype)init{
		if(self = [super init]){
	    	[self func1];
		}
		return self;
	}
	- (void)dealloc{
		[self func2];
	}
	此处略复杂。因为Clang的AST分析是DPS的，这就使得当一个消息被发送时，其一般处于一个ObjCImplDecl/ObjCCategoryDecl(此处用objcClsImpl记录类名)->ObjCMethodDecl(此处用objcSelector记录当前Sel)里面.
	因此当VisitStmt(Stmt *stmt)发现stmt是ObjCMessageExpr时，且stmt通过print得到的字符串去掉空格之后以[self开头，即当前在给self发消息。加上查看当前的objcClsImpl/objcSelector即可知道这种方法调用关系。当发现objcSelector为init开头或者dealloc时，且对self发消息时提醒用户。
	请注意这里的方法调用关系，请注意这里的方法调用关系，请注意这里的方法调用关系，此系列还将有一期关于代码量分析与优化的分享，这种调用关系即是基础之一。
		
	15.If表达式判断条件必然成立/不成立分析
	if(!10)
		NSLog(@"1");
	if(10)
		NSLog(@"2");
	if(true)
	    NSLog(@"4");
	if(false)
		NSLog(@"5");
	当VisitStmt(Stmt *stmt)中的stmt为IfStmt时，其getCond()接口返回用于判断的条件cond。针对一元表达式，cond为UnaryOperator，需要使用getSubExpr()获取新的cond并脱去一元运算符，再去分析后面的内容。对于常量条件请参考IntegerLiteral,CharacterLiteral,FloatingLiteral。结合一元表达式类型和常亮的bool内容，即可判断出If的body一定会走到或走不到并提醒用户。

## 代码FixHint功能

	clang本身提供诊断的功能，可使用
	FixItHint fixItHint = FixItHint::CreateReplacement(SourceRange(nameStart, nameEnd), replacement);
	unsigned diagID = diagEngine.getCustomDiagID(DiagnosticsEngine::Warning, "Class name should start with prefix XX");
	diagEngine.Report(location, diagID).AddFixItHint(fixItHint);
	以在指定位置location，报告错误/警告和原因，并添加替换的Hint.

![xcode-plugin-fix-hint](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/img/xcode-plugin-fix-hint.png)

## 写在后面
	如上所述，Clang插件用于风格检查，可以发现和修改的更多是一种格式上的约定和某些明显的不容许或无效逻辑，虽可解决不少问题，但是也有其局限性。实际工作中，一方面可限制使用某些固定的风格，更重要的是保持团队风格的统一和规范，提高其可读性。

[检出示例代码](https://github.com/kangwang1988/XcodeCodingStyle.git)
[Contact me](mailto:kang.wang1988@gmail.com)