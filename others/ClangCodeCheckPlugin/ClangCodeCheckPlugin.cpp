#include<iostream>
#include<sstream>

#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/AST/RecursiveASTVisitor.h"

#include "CodeCheckHelper.h"
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
                CodeCheckHelper::sharedInstance()->appendObjcCls(objcClsInterface, (interfDecl->getSuperClass()?interfDecl->getSuperClass()->getNameAsString():""),protoVec);
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
                CodeCheckHelper::sharedInstance()->appendObjcProto(objcProtocol, refProtos);
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
                    CodeCheckHelper::sharedInstance()->appendObjcClsInterf(objcClsInterface, objcIsInstanceMethod,objcSelector);
                }
                else if(objcProtocol.length()){
                    CodeCheckHelper::sharedInstance()->appendObjcProtoInterf(objcProtocol, objcIsInstanceMethod, objcSelector);
                }
                else if(objcClsImpl.length()){
                    if(methodDecl->hasBody()){
                        Stmt *methodBody = methodDecl->getBody();
                        objcMethodSrcCode.assign(context->getSourceManager().getCharacterData(methodBody->getSourceRange().getBegin()),methodBody->getSourceRange().getEnd().getRawEncoding()-methodBody->getSourceRange().getBegin().getRawEncoding()+1);
                        objcMethodFilename = context->getSourceManager().getFilename(methodBody->getSourceRange().getBegin()).str();
                        size_t pos = objcMethodFilename.find(gSrcRootPath);
                        if(pos!=string::npos){
                            objcMethodFilename = objcMethodFilename.substr(gSrcRootPath.length(),objcMethodFilename.length()-gSrcRootPath.length());
                            ostringstream stringStream;
                            stringStream<<methodBody->getSourceRange().getBegin().getRawEncoding()<<"-"<<methodBody->getSourceRange().getEnd().getRawEncoding();
                            objcMethodRange = stringStream.str();
                            CodeCheckHelper::sharedInstance()->appendObjcClsMethodImpl(objcIsInstanceMethod, objcClsImpl, objcSelector, objcMethodFilename, methodBody->getSourceRange().getBegin().getRawEncoding(),methodBody->getSourceRange().getEnd().getRawEncoding(), objcMethodSrcCode);
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
                string receiverType =objcExpr->getReceiverType().getAsString();
                size_t pos = receiverType.find(" ");
                if(pos!=string::npos){
                    receiverType = receiverType.substr(0,pos);
                }
                if(objcMethodFilename.length()){
                    CodeCheckHelper::sharedInstance()->appendObjcMethodImplCall(objcIsInstanceMethod, objcClsImpl, objcSelector,calleeIsInstanceMethod, receiverType, objcExpr->getSelector().getAsString());
                }
            }
            return true;
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
            CodeCheckHelper *helper = CodeCheckHelper::sharedInstance();
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
