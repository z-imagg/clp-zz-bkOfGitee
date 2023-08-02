#ifndef CmtAstCnsm_H
#define CmtAstCnsm_H

#include <clang/Rewrite/Core/Rewriter.h>
#include <iostream>
#include <filesystem>
#include <clang/Frontend/CompilerInstance.h>
#include "clang/AST/ASTConsumer.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Basic/SourceManager.h"
#include "clang/AST/Comment.h"
#include "clang/AST/CommentVisitor.h"


#include <fmt/core.h>

#include "Brc/BrcVst.h"
#include "Util.h"
#include "Brc/FndBrcFlagCmtHdl.h"
#include "CmtVst.h"

using namespace llvm;
using namespace clang;

//-----------------------------------------------------------------------------
// ASTConsumer
//-----------------------------------------------------------------------------


class CmtAstCnsm : public ASTConsumer {
public:
    //Rewriter:3:  Action将Rewriter传递给Consumer
    explicit CmtAstCnsm(CompilerInstance &_CI,  ASTContext *_astContext,
                             SourceManager &_SM, LangOptions &_langOptions)
            //Rewriter:4:  Consumer将Rewriter传递给Visitor
            :
            CI(_CI),
            Ctx(*_astContext),
            SM(_SM)  ,
            cmtVisitor(_CI,_astContext,_SM)
            {
      //构造函数
//      _rewriter_ptr->overwriteChangedFiles();//C'正常.
    }

    virtual bool HandleTopLevelDecl(DeclGroupRef DG) {
      for (DeclGroupRef::iterator I = DG.begin(), E = DG.end(); I != E; ++I) {
        Decl *D = *I;

          comments::FullComment *CommentAST = Ctx.getCommentForDecl(D, &(CI.getPreprocessor()));
          if (CommentAST) {
            //Ctx.getCommentForDecl(D, PP) 获得的注释 前少/* 后少*/
            //能走到这里，获取到 comments::FullComment， 并打印出注释
            Util::printSourceRangeSimple(CI,"查看注释","",CommentAST->getSourceRange(), true);


//            MyCommentVisitor CommentVisitor;
            cmtVisitor.visit(CommentAST );//能访问到自定义visitFullComment
//            cmtVisitor.visitFullComment(CommentAST);//访问不到自定义visitFullComment
          }


        RawComment *rc = Ctx.getRawCommentForDeclNoCache(D);
          if(rc){
            //Ctx.getRawCommentForDeclNoCache(D) 获得的注释是完整的
            Util::printSourceRangeSimple(CI,"查看RawComment","",rc->getSourceRange(), true);
          }
//        }
      }

      return true;
    }
    



public:
    CompilerInstance &CI;
    ASTContext & Ctx;
//    BrcVst insertVst;
//    FndBrcFlagCmtHdl findTCCallROVisitor;
    CmtVst cmtVisitor;
    SourceManager &SM;
    //两次HandleTranslationUnit的ASTConsumer只能每次新建，又期望第二次不要发生，只能让标志字段mainFileProcessed写成static
    static bool mainFileProcessed;
};


#endif