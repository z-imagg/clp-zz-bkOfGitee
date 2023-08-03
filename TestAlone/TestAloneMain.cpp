#include "clang/Lex/Lexer.h"
#include "clang/Basic/SourceManager.h"
#include "llvm/Support/CommandLine.h"
#include <iostream>



#include <clang/Frontend/FrontendActions.h>

#include "clang/Frontend/CompilerInstance.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/ArgumentsAdjusters.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Lex/PreprocessorOptions.h"
#include "clang/Basic/TargetInfo.h"
#include "Brc/BrcAstCnsm.h"
#include "clang/Parse/Parser.h"

using namespace llvm;
using namespace clang;
using namespace clang;


class MyASTConsumer : public ASTConsumer {
public:
    virtual bool HandleTopLevelDecl(DeclGroupRef DG) override {
      for (Decl *D : DG) {
        std::cout << D << std::endl;
        // 在这里处理每个声明
        // ...
      }
      return true;
    }
};


int main(int Argc, const char **Argv  ) {

  clang::CompilerInstance CI;
  CI.createDiagnostics();
  CI.createFileManager();
  CI.createSourceManager(CI.getFileManager());

  CI.getLangOpts().CPlusPlus = true;

//  LangOptions LangOpts;

  llvm::Triple triple("x86_64-pc-linux-gnu");
  std::shared_ptr<clang::TargetOptions> targetOpts=std::make_shared<clang::TargetOptions>();
  targetOpts->Triple=triple.str();

  TargetInfo* targetInfo=  TargetInfo::CreateTargetInfo(CI.getDiagnostics(), targetOpts) ;
  CI.setTarget(targetInfo);

  CI.createPreprocessor(clang::TU_Complete);
  CI.getPreprocessor().Initialize(*targetInfo);

  CI.createASTContext();

  StringRef Code ("void func(int i, int time) { return; }");

  MemoryBufferRef BufRef(Code,"exampleCPP");

  SourceManager& SM=CI.getSourceManager();

  clang::FileID MainFileID = SM.createFileID(
          BufRef, clang::SrcMgr::C_User);
    SM.setMainFileID(MainFileID);


  Preprocessor &PP = CI.getPreprocessor();
//  Lexer RawLexer(SM.getMainFileID(), BufRef, SM, LangOpts);
  Lexer RawLexer(SM.getMainFileID(), BufRef, PP, true);

//  MyASTConsumer myAstConsumer;
//  std::unique_ptr<ASTConsumer> Consumer = std::make_unique<MyASTConsumer>();
  CI.setASTConsumer(std::make_unique<MyASTConsumer>());

//  Sema seam(CI.getPreprocessor(), CI.getASTContext(),myAstConsumer );
  Sema seam(CI.getPreprocessor(), CI.getASTContext(), CI.getASTConsumer() );
  CI.setSema(&seam);

  // 创建一个Parser对象
  Parser parser(CI.getPreprocessor(), seam, false);

  parser.Initialize();
  parser.ConsumeToken();
  parser.ParseTopLevelDecl();

  ASTContext &Context = CI.getASTContext();

  TranslationUnitDecl *TUDecl = Context.getTranslationUnitDecl();
  bool hasBody = TUDecl->hasBody();
  for (Decl *D : TUDecl->decls()) {
    std::cout << D << std::endl;
    // 在这里处理每个声明
    // ...
  }
  /////




  return 0;
}
