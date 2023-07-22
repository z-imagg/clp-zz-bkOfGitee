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
    explicit CTkAstCnsm(CompilerInstance &_CI, Rewriter &R, ASTContext *Context,
                        SourceManager &SM, LangOptions &langOptions)
            //Rewriter:4:  Consumer将Rewriter传递给Visitor
            :
            CI(_CI),
            Visitor(R, Context,_CI,SM),
            findTCCallROVisitor(SM,langOptions,Context),
            SM(SM)  {}


    virtual void HandleTranslationUnit(ASTContext &Ctx) override{

      FrontendOptions &frontendOptions = CI.getFrontendOpts();
      std::cout << "frontendOptions.ProgramAction:" << frontendOptions.ProgramAction << std::endl;
      std::cout << "Ctx.TUKind:" << Ctx.TUKind << std::endl;

      FileID mainFileId = SM.getMainFileID();
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

      auto filePath=SM.getFileEntryForID(mainFileId)->getName().str();
      std::cout<<"处理编译单元,文件路径:"<<filePath<< ",mainFileId:" << mainFileId.getHashValue() << std::endl;


      //暂时 不遍历间接文件， 否则本文件会被插入两份时钟语句
      //{这样能遍历到本源文件间接包含的文件
//      TranslationUnitDecl* translationUnitDecl=Ctx.getTranslationUnitDecl();
//      Visitor.TraverseDecl(translationUnitDecl);
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

        Util::printDecl(CI, "查看", "TranslationUnitDecl.decls.j", declJ, false);


        Visitor.TraverseDecl(declJ);
        //直到第一次调用过 Visitor.TraverseDecl(declJ) 之后， Visitor.mRewriter.getRewriteBufferFor(mainFileId) 才不为NULL， 才可以用 Visitor.mRewriter 做插入动作？这是为何？
      }
      //}
//////////////////3.插入包含语句


      Util::insertIncludeToFileStart(CTkVst::IncludeStmt_TCTk,mainFileId, SM, Visitor.mRewriter);//此时  Visitor.mRewriter.getRewriteBufferFor(mainFileId)  != NULL， 可以做插入
      std::cout<< "插入'包含时钟'语句到文件头部:" << filePath << ",mainFileId:" << mainFileId.getHashValue() << std::endl;

//////////////////4.应用修改到源文件

        //不在这里写出修改，而是到 函数 EndSourceFileAction 中去 写出修改
      Visitor.mRewriter.overwriteChangedFiles();//修改会影响原始文件


    }

private:
    CompilerInstance &CI;
    CTkVst Visitor;
    FndCTkClROVst findTCCallROVisitor;
    SourceManager &SM;
};
