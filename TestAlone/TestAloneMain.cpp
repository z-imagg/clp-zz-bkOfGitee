#include "clang/Frontend/FrontendActions.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/Decl.h"
#include "clang/AST/DeclCXX.h"
#include "clang/AST/DeclVisitor.h"
#include "llvm/Support/raw_ostream.h"

class MyASTVisitor : public clang::DeclVisitor<MyASTVisitor> {
public:
    void VisitFunctionDecl(clang::FunctionDecl *FD) {
      // 处理函数声明
      llvm::outs() << "Function Decl: " << FD->getNameAsString() << "\n";
    }

    void VisitCXXRecordDecl(clang::CXXRecordDecl *RD) {
      // 处理类声明
      llvm::outs() << "CXXRecord Decl: " << RD->getNameAsString() << "\n";
    }
};

class MyASTConsumer : public clang::ASTConsumer {
public:
    void HandleTranslationUnit(clang::ASTContext &Context) override {
      MyASTVisitor Visitor;
      for (auto *D : Context.getTranslationUnitDecl()->decls()) {
        Visitor.Visit(D);
      }
    }
};

class MyASTFrontendAction : public clang::ASTFrontendAction {
public:
    std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance &CI, llvm::StringRef InFile) override {
      return std::make_unique<MyASTConsumer>();
    }
};

int main() {
  // 创建 Clang 编译实例
  clang::CompilerInstance CI;

  // 创建 ASTFrontendAction 实例
  std::unique_ptr<clang::FrontendAction> Action = std::make_unique<MyASTFrontendAction>();

  // 设置输入文件
  CI.getFrontendOpts().Inputs.push_back(clang::FrontendInputFile("/pubx/clang-brc/test_in/test_main.cpp", clang::InputKind(clang::Language::CXX)));

  // 运行 Clang 编译
  if (!CI.ExecuteAction(*Action)) {
    llvm::errs() << "Clang compilation failed\n";
    return 1;
  }

  return 0;
}