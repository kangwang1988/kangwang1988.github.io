#include<iostream>
#include<sstream>
#include<typeinfo>

#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "CodeAnalyzer.h"
#include "CodeCheckUtil.hpp"
/**
 * @discussion Enviroment:clang-3.9.1(release 39)
 * In terminal, use "/opt/llvm/llvm_build/bin/clang ../test.m -Xclang -load -Xclang lib/Debug/ClangCodeCheckPlugin.dylib -Xclang -plugin -Xclang ClangCodeCheckPlugin".
 * In Xcode, use "-Xclang -load -Xclang /opt/llvm/ClangCodeCheckPlugin/build/lib/Debug/ClangCodeCheckPlugin.dylib -Xclang -add-plugin -Xclang ClangCodeCheckPlugin",notice the -add-plugin instead of -plugin.
 *****XcodeHacking
 * sudo mv HackedClang.xcplugin `xcode-select -print-path`/../PlugIns/Xcode3Core.ideplugin/Contents/SharedSupport/Developer/Library/Xcode/Plug-ins
 * sudo mv HackedBuildSystem.xcspec `xcode-select -print-path`/Platforms/iPhoneSimulator.platform/Developer/Library/Xcode/Specifications
 * See HackedClang.xcplugin/HackedBuildSystem.xcspec @https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/others/XcodeHacking.
 */
using namespace clang;
using namespace std;
using namespace llvm;

extern string gSrcRootPath;
namespace
{
    class CodeCheckClassVisitor : public RecursiveASTVisitor<CodeCheckClassVisitor>
    {
    private:
        ASTContext *context;
        string objcClsInterface;
        string objcClsImpl;
        string objcProtocol;
        bool objcIsInstanceMethod;
        string objcSelector;
        string objcMethodSrcCode;
        string objcMethodFilename;
        string objcMethodRange;
        vector<string> hierarchy;
    public:
        void setContext(ASTContext &context)
        {
            this->context = &context;
        }
        bool VisitDecl(Decl *decl) {
            if(isa<ObjCInterfaceDecl>(decl) || isa<ObjCImplDecl>(decl) || isa<ObjCProtocolDecl>(decl)){
                objcClsInterface = string();
                objcClsImpl = string();
                objcProtocol=string();
                objcIsInstanceMethod = true;
                objcSelector = string();
                objcMethodSrcCode = string();
                objcMethodFilename = string();
                objcMethodRange = string();
                hierarchy=vector<string>();
            }
            if(isa<ObjCInterfaceDecl>(decl)){
                ObjCInterfaceDecl *interfDecl = (ObjCInterfaceDecl*)decl;
                ostringstream stringStream;
                stringStream<<interfDecl->getNameAsString();
                objcClsInterface = stringStream.str();
                vector<string> protoVec;
                for(ObjCList<ObjCProtocolDecl>::iterator it = interfDecl->all_referenced_protocol_begin();it!=interfDecl->all_referenced_protocol_end();it++){
                    protoVec.push_back((*it)->getNameAsString());
                }
                CodeAnalyzer::sharedInstance()->appendObjcCls(objcClsInterface, (interfDecl->getSuperClass()?interfDecl->getSuperClass()->getNameAsString():""),protoVec);
            }
            if(isa<ObjCProtocolDecl>(decl)){
                ObjCProtocolDecl *protoDecl = (ObjCProtocolDecl *)decl;
                ostringstream stringStream;
                stringStream<<protoDecl->getNameAsString();
                objcProtocol = stringStream.str();
                vector<string> refProtos;
                for(ObjCProtocolList::iterator it = protoDecl->protocol_begin();it!=protoDecl->protocol_end();it++){
                    refProtos.push_back((*it)->getNameAsString());
                }
                CodeAnalyzer::sharedInstance()->appendObjcProto(objcProtocol, refProtos);
            }
            if(isa<ObjCImplDecl>(decl)){
                ObjCImplDecl *interDecl = (ObjCImplDecl *)decl;
                ostringstream stringStream;
                stringStream<<interDecl->getNameAsString();
                objcClsImpl = stringStream.str();
            }
            if(isa<ObjCMethodDecl>(decl)){
                ObjCMethodDecl *methodDecl = (ObjCMethodDecl *)decl;
                ostringstream stringStream;
                objcIsInstanceMethod = methodDecl->isInstanceMethod();
                stringStream<<methodDecl->getSelector().getAsString();
                objcSelector = stringStream.str();
                if(objcClsInterface.length()){
                    CodeAnalyzer::sharedInstance()->appendObjcClsInterf(objcClsInterface, objcIsInstanceMethod,objcSelector);
                }
                else if(objcProtocol.length()){
                    CodeAnalyzer::sharedInstance()->appendObjcProtoInterf(objcProtocol, objcIsInstanceMethod, objcSelector);
                }
                else if(objcClsImpl.length()){
                    if(methodDecl->hasBody()){
                        Stmt *methodBody = methodDecl->getBody();
                        objcMethodSrcCode.assign(context->getSourceManager().getCharacterData(methodBody->getSourceRange().getBegin()),methodBody->getSourceRange().getEnd().getRawEncoding()-methodBody->getSourceRange().getBegin().getRawEncoding()+1);
                        objcMethodFilename = context->getSourceManager().getFilename(methodBody->getSourceRange().getBegin()).str();
                        if(objcMethodFilename.find(gSrcRootPath)!=string::npos){
                            objcMethodFilename = objcMethodFilename.substr(gSrcRootPath.length(),objcMethodFilename.length()-gSrcRootPath.length());
                            ostringstream stringStream;
                            stringStream<<methodBody->getSourceRange().getBegin().getRawEncoding()<<"-"<<methodBody->getSourceRange().getEnd().getRawEncoding();
                            objcMethodRange = stringStream.str();
                            CodeAnalyzer::sharedInstance()->appendObjcClsMethodImpl(objcIsInstanceMethod, objcClsImpl, objcSelector, objcMethodFilename, methodBody->getSourceRange().getBegin().getRawEncoding(),methodBody->getSourceRange().getEnd().getRawEncoding(), objcMethodSrcCode);
                        }
                    }
                }
            }
            return true;
        }
        bool VisitStmt(Stmt *s) {
            if(isa<ObjCMessageExpr>(s))
            {
                ObjCMessageExpr *objcExpr = (ObjCMessageExpr*)s;
                ostringstream stringStream;
                ObjCMessageExpr::ReceiverKind kind = objcExpr->getReceiverKind();
                bool calleeIsInstanceMethod = true;
                string calleeSel = objcExpr->getSelector().getAsString();
                string receiverType =objcExpr->getReceiverType().getAsString();
                size_t pos = receiverType.find(" ");
                if(pos!=string::npos){
                    receiverType = receiverType.substr(0,pos);
                }
                switch (kind) {
                    case ObjCMessageExpr::Class:
                    case ObjCMessageExpr::SuperClass:
                        calleeIsInstanceMethod = false;
                        break;
                    case ObjCMessageExpr::Instance:
                    case ObjCMessageExpr::SuperInstance:
                        break;
                    default:
                        break;
                }
                //无明确类型，形如id<XXXDelegate>的调用，仅处理形如id<UITableViewDataSource> dataSource, id<UITableViewDelegate> delegate，而不是id<UITableViewDataSource,UITableViewDelegate>这种。
                if(!objcExpr->getReceiverInterface()){
                    if(receiverType.find("id<")==0 && receiverType.at(receiverType.length()-1)=='>'){
                        string protocol = receiverType.substr(string("id<").length(),receiverType.length()-string("id<").length()-1);
                        int pos = protocol.find(",");
                        if(pos!=string::npos){
                            protocol = protocol.substr(0,pos);
                        }
                        protocol = trim(protocol);
                        CodeAnalyzer::sharedInstance()->appendObjcProtoInterfCall(objcIsInstanceMethod, objcClsImpl, objcSelector, protocol,calleeSel );
                    }
                    return true;
                }
                //有明确类型的调用
                string receiverInterface = objcExpr->getReceiverInterface()->getNameAsString();
                //处理通知
                if(!receiverInterface.compare("NSNotificationCenter")){
                    this->handleNotificationMessageExpr(objcExpr,calleeSel);
                }
                else if(!calleeSel.compare("performSelectorOnMainThread:withObject:waitUntilDone:modes:") ||
                        !calleeSel.compare("performSelectorOnMainThread:withObject:waitUntilDone:") ||
                        !calleeSel.compare("performSelector:onThread:withObject:waitUntilDone:modes:") ||
                        !calleeSel.compare("performSelector:onThread:withObject:waitUntilDone:") ||
                        !calleeSel.compare("performSelectorInBackground:withObject:") ||
                        !calleeSel.compare("performSelector:") ||
                        !calleeSel.compare("performSelector:withObject:") ||
                        !calleeSel.compare("performSelector:withObject:withObject:") ||
                        !calleeSel.compare("performSelector:withObject:afterDelay:inModes:") ||
                        !calleeSel.compare("performSelector:withObject:afterDelay:")){
                    this->handlePerformSelectorMessageExpr(objcExpr,calleeSel);
                }
                else if(!calleeSel.compare("new")){
                    if(!calleeIsInstanceMethod){
                        CodeAnalyzer::sharedInstance()->appendObjcMethodImplCall(objcIsInstanceMethod, objcClsImpl, objcSelector,false, receiverType,"alloc");
                        CodeAnalyzer::sharedInstance()->appendObjcMethodImplCall(objcIsInstanceMethod, objcClsImpl, objcSelector,true, receiverType,"init");
                    }
                }
                else if(objcMethodFilename.length()){
                    CodeAnalyzer::sharedInstance()->appendObjcMethodImplCall(objcIsInstanceMethod, objcClsImpl, objcSelector,calleeIsInstanceMethod, receiverType,calleeSel);
                }
            }
            return true;
        }
        void handleNotificationMessageExpr(ObjCMessageExpr *objcExpr,string calleeSel){
            LangOptions LangOpts;
            LangOpts.CPlusPlus = true;
            PrintingPolicy Policy(LangOpts);
            //此处不处理:postNotification(运行时才知道)，addObserverForName:object:queue:usingBlock(无需处理)
            if(!calleeSel.compare("addObserver:selector:name:object:")){
                string s0,s1,s2;
                raw_string_ostream param0(s0),param1(s1),param2(s2);
                objcExpr->getArg(0)->printPretty(param0, 0, Policy);
                objcExpr->getArg(1)->printPretty(param1, 0, Policy);
                objcExpr->getArg(2)->printPretty(param2, 0, Policy);
                string paramType0 = objcExpr->getArg(0)->getType().getAsString(),
                paramType1 = objcExpr->getArg(1)->getType().getAsString(),
                paramType2 = objcExpr->getArg(2)->getType().getAsString();
                string param1Sel = param1.str();
                if(param1Sel.find("@selector(")!=string::npos){
                    param1Sel = param1Sel.substr(string("@selector(").length(),param1Sel.length()-string("@selector(").length()-1);
                }
                string notif = param2.str();
                if(notif.find("@\"")!=string::npos){
                    notif = notif.substr(string("@\"").length(),notif.length()-string("@\"").length()-1);
                }
                if(!param0.str().compare("self") &&!paramType1.compare("SEL")){
                    if(!paramType2.compare("NSString *")){
                        CodeAnalyzer::sharedInstance()->appendObjcAddNotificationCall(objcIsInstanceMethod, objcClsImpl, objcSelector,objcClsImpl,param1Sel,notif);
                    }
                    else if(!paramType2.compare("NSNotificationName")){
                        CodeAnalyzer::sharedInstance()->appendObjcAddNotificationCall(objcIsInstanceMethod, objcClsImpl, objcSelector,objcClsImpl,param1Sel,param2.str());
                        CodeAnalyzer::sharedInstance()->appendObjcPostNotificationCall(true, kAppMainEntryClass, kAppMainEntrySelector,notif);
                    }
                }
            }
            else if(calleeSel.find("postNotificationName:")==0){
                string s0;
                raw_string_ostream param0(s0);
                objcExpr->getArg(0)->printPretty(param0, 0, Policy);
                string paramType0 = objcExpr->getArg(0)->getType().getAsString();
                string notif = param0.str();
                if(notif.find("@\"")!=string::npos){
                    notif = notif.substr(string("@\"").length(),notif.length()-string("@\"").length()-1);
                }
                if(!paramType0.compare("NSString *")){
                    CodeAnalyzer::sharedInstance()->appendObjcPostNotificationCall(objcIsInstanceMethod, objcClsImpl, objcSelector,notif);
                }
                else if(!paramType0.compare("NSNotificationName")){
                    CodeAnalyzer::sharedInstance()->appendObjcPostNotificationCall(true, kAppMainEntryClass, kAppMainEntrySelector,notif);
                }
            }
        }
        void handlePerformSelectorMessageExpr(ObjCMessageExpr *objcExpr,string calleeSel){
            LangOptions LangOpts;
            LangOpts.CPlusPlus = true;
            PrintingPolicy Policy(LangOpts);
            if(calleeSel.find("performSelector")==0){
                string s0;
                raw_string_ostream param0(s0);
                objcExpr->getArg(0)->printPretty(param0, 0, Policy);
                string param0Sel = param0.str();
                if(param0Sel.find("@selector(")!=string::npos){
                    param0Sel = param0Sel.substr(string("@selector(").length(),param0Sel.length()-string("@selector(").length()-1);
                }
                string receiverType = objcExpr->getReceiverType().getAsString(),receiverInterface = objcExpr->getReceiverInterface()->getNameAsString();
                //支持可明确知道类型的调用，如[self perform]/[VCClass perform]
                if(objcExpr->getReceiverInterface()){
                    CodeAnalyzer::sharedInstance()->appendObjcMethodImplCall(objcIsInstanceMethod, objcClsImpl, objcSelector, receiverType.compare(receiverInterface), receiverInterface, param0Sel);
                }
            }
        }
        void checkForLowercasedName(ObjCInterfaceDecl *declaration)
        {
            StringRef name = declaration->getName();
            char c = name[0];
            if (isLowercase(c)) {
                DiagnosticsEngine &diagEngine = context->getDiagnostics();
                unsigned diagID = diagEngine.getCustomDiagID(DiagnosticsEngine::Warning, "Class name should not start with lowercase letter");
                SourceLocation location = declaration->getLocation();
                diagEngine.Report(location, diagID);
            }
        }
        void checkForUnderscoreInName(ObjCInterfaceDecl *declaration)
        {
            size_t underscorePos = declaration->getName().find('_');
            if (underscorePos != StringRef::npos) {
                DiagnosticsEngine &diagEngine = context->getDiagnostics();
                SourceLocation location = declaration->getLocation().getLocWithOffset(underscorePos);
                unsigned diagID = diagEngine.getCustomDiagID(DiagnosticsEngine::Error, "Class name with `_` forbidden");
                diagEngine.Report(location, diagID);
            }
        }
    };
    
    class CodeCheckConsumer : public ASTConsumer
    {
    public:
        void HandleTranslationUnit(ASTContext &context){
            CodeAnalyzer *helper = CodeAnalyzer::sharedInstance();
            visitor.setContext(context);
            visitor.TraverseDecl(context.getTranslationUnitDecl());
            helper->synchronize();
        }
    private:
        CodeCheckClassVisitor visitor;
    };
    
    class CodeCheckASTAction : public PluginASTAction
    {
    public:
        unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &Compiler,llvm::StringRef InFile)
        {
            return unique_ptr<CodeCheckConsumer>(new CodeCheckConsumer);
        }
        bool ParseArgs(const CompilerInstance &CI, const std::vector<std::string>& args) {
            size_t cnt = args.size();
            if(cnt == 1){
                gSrcRootPath = args.at(0);
            }
            return true;
        }
    };
}

static clang::FrontendPluginRegistry::Add<CodeCheckASTAction>
X("ClangCodeCheckPlugin", "ClangCodeCheckPlugin");
