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

  SourceManager& SM=CI.getSourceManager();
  LangOptions &LO = CI.getLangOpts();
  Preprocessor &PP = CI.getPreprocessor();

  StringRef Code ("void func(int i, int time) { return; }");
  MemoryBufferRef BufRef(Code,"exampleCPP");
  clang::FileID MainFileID = SM.createFileID(
          BufRef, clang::SrcMgr::C_User);
  SM.setMainFileID(MainFileID);
  Lexer RawLexer(SM.getMainFileID(), BufRef, PP, true);


//  const char* FileName = "/pubx/clang-brc/test_in/test_main.cpp";
//  clang::FileID MainFileID = CI.getSourceManager().getOrCreateFileID(
//          CI.getFileManager().getVirtualFile(FileName,  0,  0),
//          clang::SrcMgr::C_User);
//  CI.getSourceManager().setMainFileID(MainFileID);
//  const clang::FileEntry *fileEntry = SM.getFileEntryForID(MainFileID);
//  llvm::StringRef fileBuffer = SM.getBufferData(MainFileID);

//  clang::Lexer lexer(SourceLocation(), LO,fileBuffer.begin(),fileBuffer.data(), fileBuffer.end() );

  PP.EnterSourceFile(MainFileID,nullptr,{},false);

  PreprocessorLexer *xx = PP.getCurrentLexer();
//  Lexer RawLexer(SM.getMainFileID(), SM, CI.getLangOpts());

//  MyASTConsumer myAstConsumer;
//  std::unique_ptr<ASTConsumer> Consumer = std::make_unique<MyASTConsumer>();
  CI.setASTConsumer(std::make_unique<MyASTConsumer>());

//  Sema seam(CI.getPreprocessor(), CI.getASTContext(),myAstConsumer );
  Sema *seam=new Sema(CI.getPreprocessor(), CI.getASTContext(), CI.getASTConsumer() );
  CI.setSema(seam);

  // 创建一个Parser对象
  Parser parser(CI.getPreprocessor(), *seam, false);
//  PP.getCurrentLexer()

  parser.Initialize();
//  parser.ConsumeToken();
  parser.ParseTopLevelDecl();

  ASTContext &Context = CI.getASTContext();

  TranslationUnitDecl *TUDecl = Context.getTranslationUnitDecl();
  bool hasBody = TUDecl->hasBody();

  for (Decl *D : TUDecl->decls()) {

    const std::string &DStr = D->getSourceRange().printToString(SM);

    llvm::raw_ostream& OS = llvm::outs();
    clang::PrintingPolicy Policy(D->getASTContext().getLangOpts());
    OS<< "D:"  << D << ",Location: " << DStr << ":";
    D->print(OS, Policy);
    OS << "\n";

  }
/**此循环输出以下信息:
D:0x555557ae00d0,Location: <<invalid sloc>>:typedef __int128 __int128_t
D:0x555557ae0140,Location: <<invalid sloc>>:typedef unsigned __int128 __uint128_t
D:0x555557ae04b8,Location: <<invalid sloc>>:typedef __NSConstantString_tag __NSConstantString
D:0x555557ae0550,Location: <<invalid sloc>>:typedef char *__builtin_ms_va_list
D:0x555557aebd68,Location: <<invalid sloc>>:typedef __va_list_tag __builtin_va_list[1]
D:0x555557aebf28,Location: <exampleCPP:1:1, col:38>:void func(int i, int time) {
    return;
}
 */



  return 0;
}
