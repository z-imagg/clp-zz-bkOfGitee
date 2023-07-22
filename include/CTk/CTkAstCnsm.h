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

class Zzz{
public:
    static void zzz(CTkVst& worker,Decl *Child) {
      const char *chKN = Child->getDeclKindName();
      Decl::Kind chK = Child->getKind();

      if (CXXRecordDecl *RD = dyn_cast<CXXRecordDecl>(Child)) {
        for (CXXMethodDecl *MD : RD->methods()) {
          Stmt *Body = MD->getBody();
          worker.TraverseStmt(Body);
        }
      }

      if (FunctionDecl *FD = dyn_cast<FunctionDecl>(Child)) {
        Stmt *Body = FD->getBody();
        Util::printStmt(*worker.Ctx, worker.CI, "上层临时查看顶层函数", "", Body, true);
//        worker.TraverseStmt(Body);
        worker.TraverseDecl(FD);
      }
      if (CXXMethodDecl *MD = dyn_cast<CXXMethodDecl>(Child)) {
        Stmt *Body = MD->getBody();
        Util::printStmt(*worker.Ctx, worker.CI, "上层临时查看c++方法", "", Body, true);
//        worker.TraverseStmt(Body);
        worker.TraverseDecl(MD);
//        worker.TraverseDecl(Body);
      }
    }
};


class NameSpaceVst : public RecursiveASTVisitor<NameSpaceVst> {
public:
    explicit NameSpaceVst(const CTkVst &cTkVst) : worker(cTkVst) {

    }

    CTkVst worker;
    bool VisitNamespaceDecl(NamespaceDecl *ND) {

      ////////本命名空间下的处理
      const DeclContext::decl_range &ds = ND->decls();
      for (Decl *Child : ds) {
        const char *chKN = Child->getDeclKindName();
        Decl::Kind chK = Child->getKind();

        Zzz::zzz(worker,Child);

      }
      ///////

      //////{递归
      for (Decl *Child : ND->decls()) {
        if (NamespaceDecl *NestedND = dyn_cast<NamespaceDecl>(Child)) {
          this->TraverseDecl(NestedND);
        }
      }
      //////

      return true;
    }
};

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

    bool HandleTopLevelDecl(DeclGroupRef DG) override {
      FileID mainFileId = SM.getMainFileID();
      auto filePath=SM.getFileEntryForID(mainFileId)->getName().str();

      FrontendOptions &frontendOptions = CI.getFrontendOpts();
      std::cout << "查看，文件路径:" << filePath << ",mainFileId:" << mainFileId.getHashValue() << ",frontendOptions.ProgramAction:" << frontendOptions.ProgramAction  <<  std::endl;


      for (Decl *D : DG) {
        const char *KN = D->getDeclKindName();
        Decl::Kind K = D->getKind();
        if (NamespaceDecl *ND = dyn_cast<NamespaceDecl>(D)) {
          NameSpaceVst namespaceVisitor(insertVst);
          namespaceVisitor.TraverseDecl(ND);
        }else{
          //试图在这里处理 不在命名空间中的 顶层c++方法体、c函数体 ： 但没成功
          Zzz::zzz(insertVst,D);
        }

      }
      return true;
    }

private:
    CompilerInstance &CI;
    CTkVst insertVst;
    FndCTkClROVst findTCCallROVisitor;
    SourceManager &SM;
};
