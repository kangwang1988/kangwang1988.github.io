#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include<iostream>
#include<sstream>
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
static string SSrcRootPath = "";
namespace
{
    class CodeCheckClassVisitor : public RecursiveASTVisitor<CodeCheckClassVisitor>
    {
    private:
        ASTContext *context;
        string objcInterfaceDecl;
        unsigned isInstanceMethod;
        string objcMethodDecl;
        string objcMethodSrcCode;
        string objcMessageExpr;
    public:
        void setContext(ASTContext &context)
        {
            this->context = &context;
        }
        bool VisitDecl(Decl *decl) {
            if(isa<ObjCInterfaceDecl>(decl)){
                ObjCInterfaceDecl *interDecl = (ObjCInterfaceDecl *)decl;
                ostringstream stringStream;
                stringStream<<interDecl->getNameAsString();
                objcInterfaceDecl = stringStream.str();
            }
            if(isa<ObjCMethodDecl>(decl)){
                ObjCMethodDecl *methodDecl = (ObjCMethodDecl *)decl;
                ostringstream stringStream;
                isInstanceMethod = methodDecl->isInstanceMethod();
                stringStream<<methodDecl->getSelector().getAsString();
                objcMethodDecl = stringStream.str();
                if(methodDecl->hasBody()){
                    Stmt *methodBody = methodDecl->getBody();
                    objcMethodSrcCode.assign(context->getSourceManager().getCharacterData(methodBody->getSourceRange().getBegin()), methodBody->getSourceRange().getEnd().getRawEncoding()-methodBody->getSourceRange().getBegin().getRawEncoding()+1);
                }
            }
            return true;
        }
        bool VisitObjCInterfaceDecl(ObjCInterfaceDecl *declaration)
        {
            return true;
        }
        bool VisitStmt(Stmt *s) {
            if(isa<ObjCMessageExpr>(s))
            {
                ObjCMessageExpr *objcExpr = (ObjCMessageExpr*)s;
                ostringstream stringStream;
                ObjCMessageExpr::ReceiverKind kind = objcExpr->getReceiverKind();
                string clsPref = "",superPref = "";
                switch (kind) {
                    case ObjCMessageExpr::Class:
                        clsPref = "+";
                        break;
                    case ObjCMessageExpr::Instance:
                        clsPref = "-";
                        break;
                    case ObjCMessageExpr::SuperClass:
                        clsPref = "+";
                        superPref = "(super)";
                        break;
                    case ObjCMessageExpr::SuperInstance:
                        clsPref = "-";
                        superPref = "(super)";
                        break;
                    default:
                        break;
                }
                string receiverType =objcExpr->getReceiverType().getAsString();
                size_t pos = receiverType.find(" ");
                if(pos!=string::npos){
                    receiverType = receiverType.substr(0,pos);
                }
                stringStream<<receiverType<<" "<<objcExpr->getSelector().getAsString();
                objcMessageExpr = stringStream.str();
                string filename = context->getSourceManager().getFilename(s->getSourceRange().getBegin()).str();
                pos = filename.find(SSrcRootPath);
                if(pos!=string::npos){
                    filename = filename.substr(SSrcRootPath.length(),filename.length()-SSrcRootPath.length());
                    cout<<filename<<endl<<(isInstanceMethod?"-":"+")<<"["<<objcInterfaceDecl<<" "<<objcMethodDecl<<"] call "<<clsPref<<"["<<objcMessageExpr<<"]"<<endl;
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
                unsigned diagID = diagEngine.getCustomDiagID(DiagnosticsEngine::Error, "[KWLM] Class name with `_` forbidden");
                diagEngine.Report(location, diagID);
            }
        }
    };
    
    class CodeCheckConsumer : public ASTConsumer
    {
    public:
        void HandleTranslationUnit(ASTContext &context) {
            visitor.setContext(context);
            visitor.TraverseDecl(context.getTranslationUnitDecl());
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
                SSrcRootPath = args.at(0);
            }
            return true;
        }
    };
}

static clang::FrontendPluginRegistry::Add<CodeCheckASTAction>
X("ClangCodeCheckPlugin", "ClangCodeCheckPlugin");
