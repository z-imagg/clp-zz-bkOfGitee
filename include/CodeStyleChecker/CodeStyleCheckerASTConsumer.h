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
      clang::FileID mainFileId = SM.getMainFileID();
      const clang::LangOptions & langOpts = Visitor.mRewriter.getLangOpts();
      //时钟函数只插入一次，不重复插入：
      //若已经有时钟函数声明，则标记为已处理，且直接返回，不做任何处理。
      //包含了时钟头文件 会有时钟函数声明。 其实是想问 是否有 时钟函数调用，但函数调用 比 声明 难找。
      //所以说 有漏洞：如果一个客户源文件，没有包含时钟头文件，但是调用了时钟函数 ，是会每次都被插入时钟语句。
      {
        std::string functionName = "X__t_clock_tick";
        //能找到 时钟滴答 函数声明
        clang::FunctionDecl* clockTickFuncDecl = CodeStyleCheckerVisitor::findFuncDecByName(&Ctx,functionName);

        //若已经有时钟函数声明
        if(clockTickFuncDecl!=NULL){

          //若已经有时钟函数声明，则标记为已处理
          CodeStyleCheckerVisitor::fileInsertedIncludeStmt.insert(mainFileId);

          //若已经有时钟函数声明，则标记为已处理，且直接返回，不做任何处理。
          return;

          //获取 时钟滴答 函数声明 源码文本，人工确定 确实是 该函数。
//          std::string clockTickFuncSourceText = CodeStyleCheckerVisitor::getSourceTextBySourceRange(clockTickFuncDecl->getSourceRange(), SM, langOpts);
//  std::cout<<clockTickFuncSourceText<<std::endl;
        }
      }

      auto filePath=SM.getFileEntryForID(SM.getMainFileID())->getName().str();
      std::cout<<"HandleTranslationUnit__filepath:"<<filePath<<std::endl;

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
