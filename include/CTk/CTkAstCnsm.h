#ifndef CTkAstCnsm_H
#define CTkAstCnsm_H

#include <clang/Rewrite/Core/Rewriter.h>
#include <iostream>
#include <clang/Frontend/CompilerInstance.h>
#include "clang/AST/ASTConsumer.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Basic/SourceManager.h"


#include "CTk/CTkVst.h"
#include "FndCTkClROVst.h"
#include "Util.h"

using namespace llvm;
using namespace clang;

//-----------------------------------------------------------------------------
// ASTConsumer
//-----------------------------------------------------------------------------



class CTkAstCnsm : public ASTConsumer {
public:
    //Rewriter:3:  Action将Rewriter传递给Consumer
    explicit CTkAstCnsm(CompilerInstance &_CI, const std::shared_ptr<Rewriter> _rewriter_ptr, ASTContext *_astContext,
                        SourceManager &_SM, LangOptions &_langOptions)
            //Rewriter:4:  Consumer将Rewriter传递给Visitor
            :
            CI(_CI),
            insertVst(_rewriter_ptr, _astContext, _CI, _SM),
            findTCCallROVisitor(_CI, _SM, _langOptions, _astContext),
            SM(_SM)  {
      //构造函数
//      _rewriter_ptr->overwriteChangedFiles();//C'正常.
    }


    virtual void HandleTranslationUnit(ASTContext &Ctx) override{
      //被上层多次调用 本方法HandleTranslationUnit，后续的调用不再进入实际处理
      if(mainFileProcessed){
        return;
      }

      FileID mainFileId = SM.getMainFileID();
      auto filePath=SM.getFileEntryForID(mainFileId)->getName().str();

      FrontendOptions &frontendOptions = CI.getFrontendOpts();
      std::cout << "查看，文件路径:" << filePath << ",mainFileId:" << mainFileId.getHashValue() << ",frontendOptions.ProgramAction:" << frontendOptions.ProgramAction << "，Ctx.TUKind:" << Ctx.TUKind <<  std::endl;

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
        findTCCallROVisitor.TraverseDecl(Decl);
      }
      //}

      if(findTCCallROVisitor.curMainFileHas_TCTkCall){
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


      Util::insertIncludeToFileStart(CTkVst::IncludeStmt_TCTk, mainFileId, SM, insertVst.mRewriter_ptr);//此时  insertVst.mRewriter.getRewriteBufferFor(mainFileId)  != NULL， 可以做插入
      std::cout<< "插入include, 插入 include时钟语句 到文件头部:" << filePath << ",mainFileId:" << mainFileId.getHashValue() << std::endl;

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


      //可以发现, 本方法 两次被调用 ， 对象地址this 即对象CTkAstCnsm的地址，两次是不同的。 原因在Act中 是 每次都是 新创建 CTkAstCnsm。
      mainFileProcessed=true;
    }

public:
    CompilerInstance &CI;
    CTkVst insertVst;
    FndCTkClROVst findTCCallROVisitor;
    SourceManager &SM;
    //两次HandleTranslationUnit的ASTConsumer只能每次新建，又期望第二次不要发生，只能让标志字段mainFileProcessed写成static
    static bool mainFileProcessed;
};


#endif