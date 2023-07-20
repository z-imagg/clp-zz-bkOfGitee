#include <clang/Rewrite/Core/Rewriter.h>
#include <iostream>
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Basic/SourceManager.h"


#include "CodeStyleChecker/CodeStyleCheckerVisitor.h"

//-----------------------------------------------------------------------------
// ASTConsumer
//-----------------------------------------------------------------------------
class CodeStyleCheckerASTConsumer : public clang::ASTConsumer {
public:
    //Rewriter:3:  Action将Rewriter传递给Consumer
    explicit CodeStyleCheckerASTConsumer(clang::Rewriter &R, clang::ASTContext *Context,
                                         clang::SourceManager &SM)
            //Rewriter:4:  Consumer将Rewriter传递给Visitor
            : Visitor(R, Context),
            SM(SM)  {}


    virtual void HandleTranslationUnit(clang::ASTContext &Ctx) override{
      auto filePath=SM.getFileEntryForID(SM.getMainFileID())->getName().str();
      std::cout<<"__filepath:"<<filePath<<std::endl;

      clang::TranslationUnitDecl* translationUnitDecl=Ctx.getTranslationUnitDecl();
      Visitor.TraverseDecl(translationUnitDecl);


//      Visitor.mRewriter.getEditBuffer(SM.getMainFileID())
//              .write(llvm::outs());

        //不在这里写出修改，而是到 函数 EndSourceFileAction 中去 写出修改
      Visitor.mRewriter.overwriteChangedFiles();//修改会影响原始文件


    }

private:
    CodeStyleCheckerVisitor Visitor;
    clang::SourceManager &SM;
    // Should this plugin be only run on the main translation unit?
};
