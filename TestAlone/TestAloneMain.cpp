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
#include <clang/Rewrite/Core/Rewriter.h>
#include <iostream>

using namespace clang;


class _Util {
public:
    static std::string _getSourceText(const clang::SourceManager& SM,const LangOptions &LO, SourceRange sourceRange){
      CharSourceRange charSourceRange=CharSourceRange::getCharRange(sourceRange);
      std::string strSourceText=Lexer::getSourceText(charSourceRange, SM, LO).str();
      return strSourceText;
    }
    static std::string _getSpelling(const clang::SourceManager& SM,const LangOptions &LO, Token token,bool *Invalid = nullptr){
      const std::string strTok = Lexer::getSpelling(token, SM, LO, Invalid);
      return strTok;
    }
    static std::tuple<int,int> extractLineAndColumn(const clang::SourceManager& SM, const clang::SourceLocation& sourceLocation ) {
      clang::PresumedLoc presumedLoc = SM.getPresumedLoc(sourceLocation);
      int line = presumedLoc.getLine();
      int column = presumedLoc.getColumn();
      return std::tuple<int,int>(line,column);
    }
    static SourceLocation getStmtEndSemicolonLocation(const Stmt *S, const SourceManager &SM,bool& endIsSemicolon) {
      const LangOptions &LO = LangOptions();
      std::string stmtText=_Util::_getSourceText(SM, LO, S->getSourceRange());
      Token JTok;

      // 获取Stmt的结束位置
      SourceLocation JLoc = S->getEndLoc();
      if(JLoc.isInvalid()){
        //如果语句末尾位置 就不合法，则标记没找到分号，再直接返回。
        endIsSemicolon= false;
        return JLoc;
      }

      do{

        Lexer::getRawToken(JLoc, JTok, SM, LO,/*IgnoreWhiteSpace:*/true);
        //忽略空白字符，IgnoreWhiteSpace：true，很关键，否则可能某个位置导致循环后还是该位置，从而死循环。
        JLoc = Lexer::getLocForEndOfToken(JTok.getEndLoc(), /*Offset*/1, SM, LO);
        //偏移量给1,Offset：1,很关键，如果不向前移动 可能循环一次还是在该位置，造成死循环。
        //取第J次循环的Token的结尾位置，JTok.getEndLoc()，很关键，否则可能下次循环还在该token上，导致死循环。
      }while (JTok.isNot(tok::semi)
      && JTok.isNot(tok::eof)
&& JTok.getLocation().isValid()
              );


      // 获取分号的结束位置

      endIsSemicolon=JTok.is(tok::semi);
      return JTok.getLocation();
    }
};

class MyASTVisitor : public RecursiveASTVisitor<MyASTVisitor> {
public:
    CompilerInstance& CI;

    explicit MyASTVisitor(CompilerInstance &_CI, const std::shared_ptr<Rewriter> _rewriter_ptr ) : CI(_CI)  {

    }
    bool VisitStmt(clang::Stmt *stmt) {
      bool endIsSemicolon=false;
      SourceManager &SM = CI.getSourceManager();
      LangOptions &LO = CI.getLangOpts();

      std::string strSourceText=_Util::_getSourceText(SM,LO,stmt->getSourceRange());

      const SourceLocation &semicolonLoc = _Util::getStmtEndSemicolonLocation(stmt, SM,endIsSemicolon);//条件断点 eq为真
      const std::string &semicolonLocStr = semicolonLoc.printToString(SM);
      llvm::outs() << "访问到语句: " << stmt->getStmtClassName()  << ": 【" << strSourceText  << "】,结尾是否分号:"<<
      std::to_string(endIsSemicolon)+""+(endIsSemicolon?(",结尾分号位置: " + semicolonLocStr):"" )
      << "\n";

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
    CompilerInstance &CI;
    MyASTVisitor visitor;
    explicit MyASTConsumer(CompilerInstance &_CI, const std::shared_ptr<Rewriter> _rewriter_ptr)  : CI(_CI) ,visitor(_CI,_rewriter_ptr)  {

    }

    void HandleTranslationUnit(clang::ASTContext &Context) override {
      for (Decl *D : Context.getTranslationUnitDecl()->decls()) {
        visitor.TraverseDecl(D);
      }


//      TranslationUnitDecl *translationUnitDecl = Context.getTranslationUnitDecl();
    }
};

class MyASTFrontendAction : public clang::ASTFrontendAction {
public:
    const std::shared_ptr<Rewriter> mRewriter_ptr=std::make_shared<Rewriter>();//这里是插件Act中的Rewriter，是源头，理应构造Rewriter.


    std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance &CI, llvm::StringRef InFile) override {

      SourceManager &SM = CI.getSourceManager();
      LangOptions &langOptions = CI.getLangOpts();
      ASTContext &astContext = CI.getASTContext();

      CI.getDiagnostics().setSourceManager(&SM);

      mRewriter_ptr->setSourceMgr(SM, langOptions);


      return std::make_unique<MyASTConsumer>(CI,mRewriter_ptr);
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