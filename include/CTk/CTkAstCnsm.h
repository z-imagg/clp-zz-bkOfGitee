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
    explicit CTkAstCnsm(CompilerInstance &_CI, Rewriter &_rewriter, ASTContext *_astContext,
                        SourceManager &_SM, LangOptions &_langOptions)
            //Rewriter:4:  Consumer将Rewriter传递给Visitor
            :
            CI(_CI),
            insertVst(_rewriter, _astContext, _CI, _SM),
            findTCCallROVisitor(_CI, _SM, _langOptions, _astContext),
            SM(_SM)  {
      //构造函数
    }


    virtual void HandleTranslationUnit(ASTContext &Ctx) override{
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

      std::cout<<"提示，开始处理编译单元,文件路径:"<<filePath<< ",mainFileId:" << mainFileId.getHashValue() << std::endl;


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

        const Decl::redecl_range &kReDeclRange = declJ->redecls();
        FileID fileId = SM.getFileID(declJ->getLocation());

        Util::printDecl(Ctx,CI, "查看", "TranslationUnitDecl.decls.j", declJ, false);


        if (NamespaceDecl *ND = dyn_cast<NamespaceDecl>(declJ)) {
          insertVst.TraverseDecl(ND);//这句话 最终会 调用  方法  insertVst.VisitNamespaceDecl
        }else{
          // 在这里处理 不在命名空间中的 顶层c++方法体、c函数体
          CTkVst::processTopNode(insertVst, declJ);
        }

//        insertVst.TraverseDecl(declJ);
        //直到第一次调用过 insertVst.TraverseDecl(declJ) 之后， insertVst.mRewriter.getRewriteBufferFor(mainFileId) 才不为NULL， 才可以用 insertVst.mRewriter 做插入动作？这是为何？
      }
      //}
//////////////////3.插入包含语句


      Util::insertIncludeToFileStart(CTkVst::IncludeStmt_TCTk, mainFileId, SM, insertVst.mRewriter);//此时  insertVst.mRewriter.getRewriteBufferFor(mainFileId)  != NULL， 可以做插入
      std::cout<< "插入include, 插入 include时钟语句 到文件头部:" << filePath << ",mainFileId:" << mainFileId.getHashValue() << std::endl;

//////////////////4.应用修改到源文件

        //不在这里写出修改，而是到 函数 EndSourceFileAction 中去 写出修改
      insertVst.mRewriter.overwriteChangedFiles();//修改会影响原始文件


    }

private:
    CompilerInstance &CI;
    CTkVst insertVst;
    FndCTkClROVst findTCCallROVisitor;
    SourceManager &SM;
};
