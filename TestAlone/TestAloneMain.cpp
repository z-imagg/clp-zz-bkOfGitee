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

//  clang::tooling::CompilationDatabase &Compilations =  * new clang::tooling::FixedCompilationDatabase(Twine("."), std::vector<std::string>());

  clang::CompilerInstance CI;
  CI.createDiagnostics();
  CI.createFileManager();
  CI.createSourceManager(CI.getFileManager());
/* 确保在创建编译器实例（`clang::CompilerInstance`）后，立即创建源管理器（`clang::SourceManager`）并将其设置到编译器实例中
否则 ，运行时 断言失败 报错 ：   CompilerInstance.h:423: clang::SourceManager& clang::CompilerInstance::getSourceManager() const: Assertion `SourceMgr && "Compiler instance has no source manager!"' failed.
 */

  CI.getLangOpts().CPlusPlus = true;

  LangOptions LangOpts;

  llvm::Triple triple("x86_64-pc-linux-gnu");
  std::shared_ptr<clang::TargetOptions> targetOpts=std::make_shared<clang::TargetOptions>();
  targetOpts->Triple=triple.str();

  TargetInfo* targetInfo=  TargetInfo::CreateTargetInfo(CI.getDiagnostics(), targetOpts) ;
  CI.setTarget(targetInfo);

  CI.createPreprocessor(clang::TU_Complete);
  CI.createASTContext();

  std::string Code = "void func(int i, int time)"
                     "{"
                     "return;"
                     "}";

  // 创建一个MemoryBuffer对象
//  std::unique_ptr<llvm::MemoryBuffer> Buf =
//          llvm::MemoryBuffer::getMemBuffer(Code/*,"input.cpp"*/);

//  MemoryBufferRef BufRef(*Buf);
  MemoryBufferRef BufRef(Code,"input.cpp");

  SourceManager& SM=CI.getSourceManager();
  FileManager &FM = CI.getFileManager();
  // 获取输入文件
//  const FileEntry *File = CI.getFileManager().getVirtualFile("input.cpp", Buf->getBufferSize(), 0);
//  FileID MainFileID = SM.createFileID(File, SourceLocation(), SrcMgr::C_User);
//  SM.setMainFileID(MainFileID);

//  FileID MainFileID = SM.createMainFileIDForMemBuffer(std::move(Buf));
//  SM.setMainFileID(MainFileID);

  // 创建一个输入文件缓冲区
//  clang::FileID MainFileID =
//  SM.getOrCreateFileID( File, clang::SrcMgr::C_User);
  clang::FileID MainFileID = SM.createFileID(
          BufRef, clang::SrcMgr::C_User);
    SM.setMainFileID(MainFileID);


//  CI.createPreprocessor(clang::TU_Complete);
  // 创建一个Lexer对象
  Lexer RawLexer(SM.getMainFileID(), BufRef, SM, LangOpts);

//  MyASTConsumer myAstConsumer;
//  std::unique_ptr<ASTConsumer> Consumer = std::make_unique<MyASTConsumer>();
  CI.setASTConsumer(std::make_unique<MyASTConsumer>());

//  Sema seam(CI.getPreprocessor(), CI.getASTContext(),myAstConsumer );
  Sema seam(CI.getPreprocessor(), CI.getASTContext(), CI.getASTConsumer() );


  // 创建一个Parser对象
  Parser parser(CI.getPreprocessor(), seam, false);

  Preprocessor &PP = CI.getPreprocessor();
  auto _ObjC=CI.getLangOpts().ObjC;
  IdentifierTable &identifierTable = PP.getIdentifierTable();
  IdentifierInfo &_super = PP.getIdentifierTable().get("super");
  LangOptions &LO = CI.getLangOpts();
  unsigned int _AltiVec = LO.AltiVec;
  unsigned int _ZVector = LO.ZVector;
  unsigned int _Borland = LO.Borland;
  unsigned int _CPlusPlusModules = LO.CPlusPlusModules;

  // 解析代码

  parser.Initialize();
  parser.ConsumeToken();
  parser.ParseTopLevelDecl();

  // 获取ASTContext
  ASTContext &Context = CI.getASTContext();

  // 输出结果
  TranslationUnitDecl *TUDecl = Context.getTranslationUnitDecl();
  bool hasBody = TUDecl->hasBody();
  for (Decl *D : TUDecl->decls()) {
    std::cout << D << std::endl;
    // 在这里处理每个声明
    // ...
  }
  /////



//  clang::FileID MainFileID = CI.getSourceManager().createFileID(
//          llvm::MemoryBuffer::getMemBuffer(Code), clang::SrcMgr::C_User);
//
//  SourceLocation StartLoc = SM.getLocForStartOfFile(MainFileID);
//  const std::string &StartLocStr = StartLoc.printToString(SM);
//
//  SourceLocation StartLoc_getLocForEndOfToken = Lexer::getLocForEndOfToken(StartLoc, 0, SM, LangOptions());
//  const std::string &StartLoc_getLocForEndOfToken_Str = StartLoc_getLocForEndOfToken.printToString(SM);
//
//  SourceLocation EndLoc = SM.getLocForEndOfFile(MainFileID);
//  const std::string &EndLocStr = EndLoc.printToString(SM);
//
//  SourceLocation EndLoc_getLocForEndOfToken = Lexer::getLocForEndOfToken(EndLoc, 0, SM, LangOptions());
//  const std::string &EndLoc_getLocForEndOfToken_Str = EndLoc_getLocForEndOfToken.printToString(SM);
//
//  // 输出结果
//  std::cout << "代码:"  << std::endl;
//  std::cout <<  Code << std::endl;
//  std::cout << "StartLocStr:" << StartLocStr << std::endl;
//  std::cout << "getLocForEndOfToken(StartLoc,0):" << StartLoc_getLocForEndOfToken_Str << std::endl;
//  std::cout << "EndLocStr:" << EndLocStr << std::endl;
//  std::cout << "getLocForEndOfToken(EndLoc,0):" << EndLoc_getLocForEndOfToken_Str << std::endl;

  return 0;
}
