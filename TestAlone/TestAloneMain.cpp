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
#include "Brc/BrcAstCnsm.h"

using namespace llvm;
using namespace clang;
using namespace clang;



int main(int Argc, const char **Argv  ) {

  // 获取编译配置
  clang::tooling::CompilationDatabase &Compilations =  * new clang::tooling::FixedCompilationDatabase(Twine("."), std::vector<std::string>());

  // 创建 CompilerInstance 对象
  clang::CompilerInstance CI;
  // 创建源管理器
  CI.createDiagnostics();
  CI.createFileManager();
  CI.createSourceManager(CI.getFileManager());
/* 确保在创建编译器实例（`clang::CompilerInstance`）后，立即创建源管理器（`clang::SourceManager`）并将其设置到编译器实例中
否则 ，运行时 断言失败 报错 ：   CompilerInstance.h:423: clang::SourceManager& clang::CompilerInstance::getSourceManager() const: Assertion `SourceMgr && "Compiler instance has no source manager!"' failed.
 */

  // 设置语言选项
  CI.getLangOpts().CPlusPlus = true;

  SourceManager& SM=CI.getSourceManager();


  std::string Code = "int x = 5;";

  clang::FileID MainFileID = CI.getSourceManager().createFileID(
          llvm::MemoryBuffer::getMemBuffer(Code), clang::SrcMgr::C_User);




  SourceLocation StartLoc = SM.getLocForStartOfFile(MainFileID);
  const std::string &StartLocStr = StartLoc.printToString(SM);

  SourceLocation StartLoc_getLocForEndOfToken = Lexer::getLocForEndOfToken(StartLoc, 0, SM, LangOptions());
  const std::string &StartLoc_getLocForEndOfToken_Str = StartLoc_getLocForEndOfToken.printToString(SM);

  SourceLocation EndLoc = SM.getLocForEndOfFile(MainFileID);
  const std::string &EndLocStr = EndLoc.printToString(SM);

  SourceLocation EndLoc_getLocForEndOfToken = Lexer::getLocForEndOfToken(EndLoc, 0, SM, LangOptions());
  const std::string &EndLoc_getLocForEndOfToken_Str = EndLoc_getLocForEndOfToken.printToString(SM);

  // 输出结果
  std::cout << "代码:"  << std::endl;
  std::cout <<  Code << std::endl;
  std::cout << "StartLocStr:" << StartLocStr << std::endl;
  std::cout << "getLocForEndOfToken(StartLoc,0):" << StartLoc_getLocForEndOfToken_Str << std::endl;
  std::cout << "EndLocStr:" << EndLocStr << std::endl;
  std::cout << "getLocForEndOfToken(EndLoc,0):" << EndLoc_getLocForEndOfToken_Str << std::endl;

  return 0;
}
