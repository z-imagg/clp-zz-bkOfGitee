#include <clang/Rewrite/Core/Rewriter.h>
#include <iostream>
#include "clang/AST/ASTConsumer.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Basic/SourceManager.h"


#include "CodeStyleChecker/CodeStyleCheckerVisitor.h"
#include "FindTClkCall_ReadOnly_Visitor.h"

//-----------------------------------------------------------------------------
// ASTConsumer
//-----------------------------------------------------------------------------
class CodeStyleCheckerASTConsumer : public clang::ASTConsumer {
public:
    //Rewriter:3:  Action将Rewriter传递给Consumer
    explicit CodeStyleCheckerASTConsumer(clang::Rewriter &R, clang::ASTContext *Context,
                                         clang::SourceManager &SM,clang::LangOptions &langOptions)
            //Rewriter:4:  Consumer将Rewriter传递给Visitor
            : Visitor(R, Context),
            findTCCallROVisitor(SM,langOptions,Context),
            SM(SM)  {}


    virtual void HandleTranslationUnit(clang::ASTContext &Ctx) override{
      clang::FileID mainFileId = SM.getMainFileID();
      const clang::LangOptions & langOpts = Visitor.mRewriter.getLangOpts();

      //时钟函数只插入一次，不重复插入：
      //若已经有时钟函数调用，则标记为已处理，且直接返回，不做任何处理。
      {
      //{本循环遍历直接在本源文件中的函数调用们
      auto Decls = Ctx.getTranslationUnitDecl()->decls();
      for (auto &Decl : Decls) {
        if (!SM.isInMainFile(Decl->getLocation())){
          continue;
        }
        findTCCallROVisitor.TraverseDecl(Decl);
      }
      //}

      if(findTCCallROVisitor.curMainFileHas_TCTickCall){
        //若已经有时钟函数调用，则标记为已处理，且直接返回，不做任何处理。
        return;
      }
      }



      auto filePath=SM.getFileEntryForID(mainFileId)->getName().str();
      std::cout<<"处理编译单元,文件路径:"<<filePath<< ",mainFileId:" << mainFileId.getHashValue() << std::endl;

//      Ctx.getTranslationUnitDecl()->getBeginLoc() 的 FileId 居然==0?  而mainFileId==1

      //暂时 不遍历间接文件， 否则本文件会被插入两份时钟语句
      //{这样能遍历到本源文件间接包含的文件
//      clang::TranslationUnitDecl* translationUnitDecl=Ctx.getTranslationUnitDecl();
//      Visitor.TraverseDecl(translationUnitDecl);
      //}

      //{本循环能遍历到直接在本源文件中的函数定义中
      auto Decls = Ctx.getTranslationUnitDecl()->decls();
      for (auto &Decl : Decls) {
        if (!SM.isInMainFile(Decl->getLocation())){
          continue;
        }
        Visitor.TraverseDecl(Decl);
        //直到第一次调用过 Visitor.TraverseDecl(Decl) 之后， Visitor.mRewriter.getRewriteBufferFor(mainFileId) 才不为NULL， 才可以用 Visitor.mRewriter 做插入动作？这是为何？
      }
      //}



      CodeStyleCheckerVisitor::insertIncludeToFileStart(mainFileId, SM, Visitor.mRewriter);//此时  Visitor.mRewriter.getRewriteBufferFor(mainFileId)  != NULL， 可以做插入
      std::cout<< "插入'包含时钟'语句到文件头部:" << filePath << ",mainFileId:" << mainFileId.getHashValue() << std::endl;

        //不在这里写出修改，而是到 函数 EndSourceFileAction 中去 写出修改
      Visitor.mRewriter.overwriteChangedFiles();//修改会影响原始文件


    }

private:
    CodeStyleCheckerVisitor Visitor;
    FindTClkCall_ReadOnly_Visitor findTCCallROVisitor;
    clang::SourceManager &SM;
};
