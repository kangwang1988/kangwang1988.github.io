#include <iostream>
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/AST/RecursiveASTVisitor.h"

using namespace clang;
using namespace std;
using namespace llvm;

namespace ClangPlugin
{
    class ClangPluginASTVisitor : public RecursiveASTVisitor<ClangPluginASTVisitor>
    {
    private:
        ASTContext *context;
    public:
        void setContext(ASTContext &context){
                this->context = &context;
        }
        bool VisitDecl(Decl *decl){
            if(isa<ObjCImplDecl>(decl)){
                ObjCImplDecl *interDecl = (ObjCImplDecl*)decl;
                cout<<"[KWLM]Class Implementation Found:"<<interDecl->getNameAsString()<<endl;
            }
            return true;
        }
    };
    class ClangPluginASTConsumer : public ASTConsumer
    {
    private:
        ClangPluginASTVisitor visitor;
        void HandleTranslationUnit(ASTContext &context){
            visitor.setContext(context);
            visitor.TraverseDecl(context.getTranslationUnitDecl());
        }
    };
    
    class ClangPluginASTAction : public PluginASTAction
    {
    public:
        unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &Compiler,StringRef InFile){
            return unique_ptr<ClangPluginASTConsumer>(new ClangPluginASTConsumer);
        }
        
        bool ParseArgs(const CompilerInstance &CI, const std::vector<std::string>& args){
            return true;
        }
    };
}

static clang::FrontendPluginRegistry::Add<ClangPlugin::ClangPluginASTAction>
X("ClangPlugin", "ClangPlugin");
