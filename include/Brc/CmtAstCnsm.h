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

        // 获取注释
//        const RawComment *RC = D->getASTContext().getRawCommentForDeclNoCache(D);
//        const RawComment *RC = D->getASTContext().getCommentForDecl(D);

//        if (RC) {getRawCommentForDeclNoCache
          // 创建评论AST节点
          comments::FullComment *CommentAST = Ctx.getCommentForDecl(D, &(CI.getPreprocessor()));
//        Ctx.addComment()
          // 遍历评论AST节点
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
    
/*    bool HandleTopLevelDecl(DeclGroupRef DG) override{
//      return ASTConsumer::HandleTopLevelDecl(D);
      std::vector dg(DG.begin(),DG.end());
      unsigned long dgSize = dg.size();
      for (Decl *D : DG) {
        cmtVisitor.visit(D);
        __HandleDecl(D);
      }

    }*/

/*
    void __HandleDecl(Decl *D) {
//      if (FunctionDecl *FD = dyn_cast<FunctionDecl>(D)) {
//        __HandleFunctionDecl(FD);
//      }

      ASTContext &astContext = D->getASTContext();
      Preprocessor &PP = CI.getPreprocessor();
      RawComment *C0 = astContext.getRawCommentForDeclNoCache(D);
//      comments::CommandTraits &commandTraits = astContext.getCommentCommandTraits();
      comments::FullComment *C = astContext.getCommentForDecl(D, &PP);
      Util::printDecl(Ctx,CI,"查看源码","",D,true);
      if (C) {
        __HandleComment(C);
      }
    }
*/

/*    void __HandleFunctionDecl(FunctionDecl *FD) {
//      SourceManager &SM = Ctx.getSourceManager();
      SourceRange CommentRange = FD->getCommentRange();
      if (SM.isInMainFile(CommentRange.getBegin())) {
        std::string CommentText = Lexer::getSourceText(CharSourceRange::getTokenRange(CommentRange), SM, Context->getLangOpts()).str();
        llvm::outs() << "Function Comment: " << CommentText << "\n";
      }
    }*/

    void __HandleComment(comments::FullComment * C) {
//      SourceManager &SM = Context->getSourceManager();

      SourceRange CommentRange = C->getSourceRange();
      if (SM.isInMainFile(CommentRange.getBegin())) {
        std::string CommentText = Lexer::getSourceText(CharSourceRange::getTokenRange(CommentRange), SM, Ctx.getLangOpts()).str();
        llvm::outs() << "Top-level Comment: " << CommentText << "\n";
      }
    }


/*
    virtual void HandleTranslationUnit(ASTContext &Ctx) override{
      //被上层多次调用 本方法HandleTranslationUnit，后续的调用不再进入实际处理
      if(mainFileProcessed){
        return;
      }

      FileID mainFileId;
      std::string filePath;
      Util::getMainFileIDMainFilePath(SM,mainFileId,filePath);

      //若是系统文件 或 tick文件则跳过
      if(Util::isSysSrcFile(filePath)  || Util::isTickSrcFile(filePath)){
        return ;
      }

      FrontendOptions &frontendOptions = CI.getFrontendOpts();
      std::cout << "查看，文件路径:" << filePath << ",mainFileId:" << mainFileId.getHashValue() << ",frontendOptions.ProgramAction:" << frontendOptions.ProgramAction << "，Ctx.TUKind:" << Ctx.TUKind <<  std::endl;

      //复制源文件 到 /build/srcCopy/, 开关copySrcFile=true.
      // (适合cmake测试编译器，源文件用完即删除，导致此时出问题后拿不到源文件，难以复现问题）
      if(Util::envVarEq("copySrcFile","true")){
        Util::copySrcFile(filePath,"/build/srcCopy/");
      }



//////////////////1. 若已插入 ，则不用处理
      //时钟函数只插入一次，不重复插入：
      //若已经有时钟函数调用，则标记为已处理，且直接返回，不做任何处理。
      {
      //{本循环遍历直接在本源文件中的函数调用们
      auto Decls = Ctx.getTranslationUnitDecl()->decls();
      for (auto &Decl : Decls) {
        if (!SM.isInMainFile(Decl->getLocation())){
          continue;
        }
        findTCCallROVisitor.HandleComment(CI.getPreprocessor(),Ctx.getCommentCommandTraits());

        findTCCallROVisitor.TraverseDecl(Decl);
      }
      //}

      if(findTCCallROVisitor.curMainFileHas_BrcInsrtFlag){
        //若已经有时钟函数调用，则标记为已处理，且直接返回，不做任何处理。
        return;
      }
      }

//////////////////2. 插入时钟语句

      std::cout<<"提示，开始处理编译单元,文件路径:"<<filePath<< ",CTkAstConsumer:" << this << ",mainFileId:" << mainFileId.getHashValue() << std::endl;

      //暂时 不遍历间接文件， 否则本文件会被插入两份时钟语句
      //{这样能遍历到本源文件间接包含的文件
//      TranslationUnitDecl* translationUnitDecl=Ctx.getTranslationUnitDecl();
//      insertVst.TraverseDecl(translationUnitDecl);
      //}

      //{本循环能遍历到直接在本源文件中的函数定义中
      const DeclContext::decl_range &Decls = Ctx.getTranslationUnitDecl()->decls();
      //const DeclContext::decl_iterator::value_type &declK
      for (clang::Decl* declJ : Decls) {
        if (!SM.isInMainFile(declJ->getLocation())){
          continue;
        }

        FileID fileId = SM.getFileID(declJ->getLocation());

//        Util::printDecl(Ctx,CI, "查看", "TranslationUnitDecl.decls.j", declJ, false);


        insertVst.TraverseDecl(declJ);
      }
      //}

//////////////////3.插入包含语句

      bool insertResult;
      Util::insertIncludeToFileStart(CTkVst::IncludeStmt_TCTk, mainFileId, SM, insertVst.mRewriter_ptr,insertResult);//此时  insertVst.mRewriter.getRewriteBufferFor(mainFileId)  != NULL， 可以做插入
      std::string msg=fmt::format("插入include到文件{},对mainFileId:{},结果:{}\n",filePath,mainFileId.getHashValue(),insertResult);
      std::cout<< msg ;

//////////////////4.应用修改到源文件


      //overwriteChangedFiles引发 "1.	<eof> parser at end of file" 并以  "Program received signal SIGSEGV, Segmentation fault." 退出， 可能原因是修改后的源码有语法错误，侦察错误办法是 overwriteChangedFiles 之前 先调用getRewrittenText获得改后的源码文本，人工查看哪里有语法错误。
//      const std::string &text = insertVst.mRewriter_ptr->getRewrittenText( Ctx.getTranslationUnitDecl()->getSourceRange());
      if(Util::envVarEq("saveTextBefore_overwriteChangedFiles","true")){
        Util::saveRewriteBuffer(insertVst.mRewriter_ptr, mainFileId, filePath + ".getRewriteBufferFor");
        Util::saveEditBuffer(insertVst.mRewriter_ptr, mainFileId, filePath + ".getEditBuffer");
      }

      //CI.getDiagnostics().dump()会输出 本次编译文件、其包含的各个文件 诊断信息.
      if(Util::envVarEq("diagnostics_dump","true")){
        DiagnosticsEngine &diagnostics = CI.getDiagnostics();
        diagnostics.dump();
      }

      insertVst.mRewriter_ptr->overwriteChangedFiles();


      //可以发现, 本方法 两次被调用 ， 对象地址this 即对象CTkAstCnsm的地址，两次是不同的。 原因在Act中 是 每次都是 新创建 CmtAstCnsm。
      mainFileProcessed=true;
    }
*/

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