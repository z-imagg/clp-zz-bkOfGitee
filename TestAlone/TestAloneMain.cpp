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
#include <clang/AST/RecursiveASTVisitor.h>

using namespace clang;


class Util {
public:
    static SourceLocation getStmtEndSemicolonLocation(const Stmt *S, const SourceManager &SM) {
      // 获取Stmt的结束位置
      const SourceLocation EndLoc = S->getEndLoc();

      // 获取下一个token的结束位置
      SourceLocation NextTokenEndLoc = Lexer::getLocForEndOfToken(EndLoc, 0, SM, LangOptions());

      // 查找下一个分号
      Token Tok;
      Lexer::getRawToken(NextTokenEndLoc, Tok, SM, LangOptions());

      while (Tok.isNot(tok::semi) && Tok.isNot(tok::eof)) {
        NextTokenEndLoc = Lexer::getLocForEndOfToken(Tok.getLocation(), 0, SM, LangOptions());
        Lexer::getRawToken(NextTokenEndLoc, Tok, SM, LangOptions());
      }

      // 获取分号的结束位置
      SourceLocation SemicolonEndLoc = Lexer::getLocForEndOfToken(Tok.getLocation(), 0, SM, LangOptions());

      return SemicolonEndLoc;
    }
};

class MyASTVisitor : public RecursiveASTVisitor<MyASTVisitor> {
public:
    bool VisitStmt(clang::Stmt *stmt) {
      llvm::outs() << "VisitStmt: " << stmt->getStmtClassName() << "\n";
      return true;
    }
/*输出:
TraverseStmt: ImplicitCastExpr
TraverseStmt: FloatingLiteral
TraverseStmt: CompoundStmt
TraverseStmt: ReturnStmt
TraverseStmt: BinaryOperator
TraverseStmt: ImplicitCastExpr
TraverseStmt: DeclRefExpr
TraverseStmt: ImplicitCastExpr
TraverseStmt: DeclRefExpr
 */
};

class MyASTConsumer : public clang::ASTConsumer {
public:
    void HandleTranslationUnit(clang::ASTContext &Context) override {
      MyASTVisitor Visitor;
      for (Decl *D : Context.getTranslationUnitDecl()->decls()) {
        Visitor.TraverseDecl(D);
      }


//      TranslationUnitDecl *translationUnitDecl = Context.getTranslationUnitDecl();
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