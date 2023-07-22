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

class NamespaceVisitor : public RecursiveASTVisitor<NamespaceVisitor> {
public:
    explicit NamespaceVisitor(const CTkVst &cTkVst) : worker(cTkVst) {

    }

    CTkVst worker;
    bool VisitNamespaceDecl(NamespaceDecl *ND) {
//      if (ND->getQualifiedNameAsString() == "myNs1::myNs2") {
        // 遍历命名空间中的声明
//        DeclarationVisitor Visitor;
//        for (Decl *Ch : ND->decls()) {
          const DeclContext::decl_range &ds = ND->decls();

          for (Decl *Child : ds) {
            const char *chKN = Child->getDeclKindName();
            Decl::Kind chK = Child->getKind();

            if (CXXRecordDecl *RD = dyn_cast<CXXRecordDecl>(Child)) {
              for (CXXMethodDecl *MD : RD->methods()) {
                // 遍历函数体内的语句
                worker.TraverseStmt(MD->getBody());
//                worker.TraverseDecl(Child);
              }
            }
          }
//          worker.TraverseDecl(Ch);
//        }


//      }
      // 递归遍历嵌套的命名空间
      for (Decl *Child : ND->decls()) {
        if (NamespaceDecl *NestedND = dyn_cast<NamespaceDecl>(Child)) {
//          NamespaceVisitor Visitor;
          this->TraverseDecl(NestedND);
        }
      }
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
            Visitor(_rewriter, _astContext, _CI, _SM),
            findTCCallROVisitor(_CI, _SM, _langOptions, _astContext),
            SM(_SM)  {
      //构造函数
    }

    bool HandleTopLevelDecl(DeclGroupRef DG) override {
      for (Decl *D : DG) {
        if (NamespaceDecl *ND = dyn_cast<NamespaceDecl>(D)) {
          NamespaceVisitor namespaceVisitor(Visitor);
          namespaceVisitor.TraverseDecl(ND);
        }
      }
      return true;
    }
/*    bool HandleTopLevelDecl(DeclGroupRef DG) override {
      for (Decl *D : DG) {
        const char *dKN = D->getDeclKindName();
        Decl::Kind dK = D->getKind();
        if (NamespaceDecl *ND = dyn_cast<NamespaceDecl>(D)) {
          const std::string &nDN = ND->getNameAsString();
          const std::string &nDQN = ND->getQualifiedNameAsString();
          const DeclContext::decl_range &ds = ND->decls();
            for (Decl *Child : ds) {
              const char *chKN = Child->getDeclKindName();
              Decl::Kind chK = Child->getKind();
              
              if (CXXRecordDecl *RD = dyn_cast<CXXRecordDecl>(Child)) {
                  for (CXXMethodDecl *MD : RD->methods()) {
                      // 遍历函数体内的语句
                      Visitor.TraverseStmt(MD->getBody());
                  }
              }
            }
        }
      }
//      return true;


      for (Decl *D : DG) {
        if (FunctionDecl *FD = dyn_cast<FunctionDecl>(D)) {
          Visitor.TraverseStmt(FD->getBody());
        }
      }
      return true;
    }*/

private:
    CompilerInstance &CI;
    CTkVst Visitor;
    FndCTkClROVst findTCCallROVisitor;
    SourceManager &SM;
};
