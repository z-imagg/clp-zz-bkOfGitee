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

        if (CXXRecordDecl *RD = dyn_cast<CXXRecordDecl>(Child)) {
          for (CXXMethodDecl *MD : RD->methods()) {
            worker.TraverseStmt(MD->getBody());
          }
        }

        if (FunctionDecl *FD = dyn_cast<FunctionDecl>(Child)) {
          worker.TraverseStmt(FD->getBody());
        }

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
        if (NamespaceDecl *ND = dyn_cast<NamespaceDecl>(D)) {
          NameSpaceVst namespaceVisitor(insertVst);
          namespaceVisitor.TraverseDecl(ND);
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
