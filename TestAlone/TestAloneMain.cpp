#include "clang/Frontend/FrontendActions.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/Decl.h"
#include "clang/AST/DeclCXX.h"
#include "clang/AST/DeclVisitor.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/Basic/TargetInfo.h"
#include <clang/Frontend/TextDiagnosticPrinter.h>

using namespace clang;

class MyASTVisitor : public clang::DeclVisitor<MyASTVisitor> {
public:
    void VisitFunctionDecl(clang::FunctionDecl *FD) {
      llvm::outs() << "FunctionDecl: " << FD->getNameAsString() << "\n";
    }
    void VisitCXXMethodDecl(clang::CXXMethodDecl *CMD) {
      llvm::outs() << "CXXMethodDecl: " << CMD->getNameAsString() << "\n";
    }
    void VisitCXXRecordDecl(clang::CXXRecordDecl *RD) {
      llvm::outs() << "CXXRecordDecl: " << RD->getNameAsString() << "\n";
    }
/**输出:
CXXRecordDecl: MyClass
CXXMethodDecl: myFunction

/pubx/clang-brc/test_in/test_main.cpp:7:19: warning: implicit conversion from 'double' to 'int' changes value from 0.001 to 0
int MyClass::ZERO=0.001;
             ~~~~ ^~~~~
*/
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

  CI.createDiagnostics();
  CI.createFileManager();
  CI.createSourceManager(CI.getFileManager());

  CI.getLangOpts().CPlusPlus = true;

  CI.getTargetOpts().Triple= "x86_64-pc-linux-gnu";

  llvm::Triple triple("x86_64-pc-linux-gnu");
  std::shared_ptr<clang::TargetOptions> targetOpts=std::make_shared<clang::TargetOptions>();
  targetOpts->Triple=triple.str();
  TargetInfo* targetInfo=  TargetInfo::CreateTargetInfo(CI.getDiagnostics(), targetOpts) ;
  CI.setTarget(targetInfo);

  CI.createPreprocessor(clang::TU_Complete);
  CI.getPreprocessor().Initialize(*targetInfo);


  SourceManager& SM=CI.getSourceManager();
  LangOptions &LO = CI.getLangOpts();
  Preprocessor &PP = CI.getPreprocessor();

  //添加诊断
  llvm::raw_ostream &OS = llvm::outs();
  DiagnosticOptions *diagnosticOptions = new clang::DiagnosticOptions();
  clang::TextDiagnosticPrinter *TextDiag = new TextDiagnosticPrinter(OS, diagnosticOptions);
  TextDiag->BeginSourceFile(LO,&PP);
//  TextDiag->EndSourceFile();
  CI.getDiagnostics().setClient(TextDiag);

  // 创建 ASTFrontendAction 实例
  clang::FrontendAction* Action = new MyASTFrontendAction();

  // 设置输入文件
  CI.getFrontendOpts().Inputs.push_back(clang::FrontendInputFile("/pubx/clang-brc/test_in/test_main.cpp", clang::InputKind(clang::Language::CXX)));

  // 运行 Clang 编译
  if (!CI.ExecuteAction(*Action)) {
    llvm::errs() << "Clang compilation failed\n";
    return 1;
  }

  return 0;
}