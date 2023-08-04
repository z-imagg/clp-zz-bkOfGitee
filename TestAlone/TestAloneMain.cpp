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
#include "clang/Lex/MacroInfo.h"
#include <vector>

#include <iostream>

using namespace clang;



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
  clang::FrontendAction* Action = new clang::SyntaxOnlyAction();

  // 设置输入文件
  CI.getFrontendOpts().Inputs.push_back(clang::FrontendInputFile("/pubx/clang-brc/test_in/test_main.cpp", clang::InputKind(clang::Language::CXX)));

  // 运行 Clang 编译
  if (!CI.ExecuteAction(*Action)) {
    llvm::errs() << "Clang compilation failed\n";
    return 1;
  }

  Preprocessor &_PP = CI.getPreprocessor();
  //注意不能用上面的PP,会报指针问题，重新getPreprocessor得到的_PP可以正常使用

//region 获取宏名、宏展开后文本
  for (clang::Preprocessor::macro_iterator I = _PP.macro_begin(), E = _PP.macro_end(); I != E; ++I) {
    const clang::IdentifierInfo *II = I->first;
    const StringRef &IdentifierName = II->getName();
    const clang::MacroInfo *MI = _PP.getMacroInfo(II);
    //不显示内置宏
    if(MI->isBuiltinMacro()){
      continue;
    }
    //只显示本文件中定义的宏，开发用
    if(!SM.isWrittenInMainFile(MI->getDefinitionLoc())){
      continue;
    }
    if (MI) {
      // 获取宏展开后的标记序列
//      clang::MacroInfo::tokens_iterator TI = MI->tokens_begin();
//      clang::MacroInfo::tokens_iterator TE = MI->tokens_end();
      const Token* TI = MI->tokens_begin();
      const Token* TE = MI->tokens_end();

      // 将标记序列转换为文本
      std::string macroText;
      for (; TI != TE; ++TI) {
        const clang::Token &token = *TI;
        const StringRef &tokenText = clang::Lexer::getSourceText(clang::CharSourceRange::getTokenRange(token.getLocation()), SM, LO);
        macroText += (" "+ tokenText.str() );
      }

      // 打印宏展开后的文本
      std::cout << "宏名字:【" << IdentifierName.str() << "】，宏展开:【" << macroText <<"】"<< std::endl;
    }
  }

//endregion

  return 0;
}