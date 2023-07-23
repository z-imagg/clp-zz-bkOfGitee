#include "CTk/CTkAstCnsm.h"

#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/ArgumentsAdjusters.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Lex/PreprocessorOptions.h"

using namespace llvm;
using namespace clang;

//===----------------------------------------------------------------------===//
// Command line options
//===----------------------------------------------------------------------===//
static llvm::cl::OptionCategory CTkAloneCategory("CTkAlone options");

//===----------------------------------------------------------------------===//
// PluginASTAction
//===----------------------------------------------------------------------===//
class CTkAloneAct : public PluginASTAction {
public:
  bool ParseArgs(const CompilerInstance &CI,
                 const std::vector<std::string> &args) override {
    return true;
  }

  //本方法是override的 即 上层定的，只能返回 std::unique_ptr<ASTConsumer>，因此只能每次新创建CTkAstCnsm， 而不能每次给一个固定的CTkAstCnsm对象
  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 StringRef file) override {
    SourceManager& SM=CI.getSourceManager();
    LangOptions &langOpts=CI.getLangOpts();
    ASTContext& astContext=CI.getASTContext();
    //Rewriter:2:  Rewriter构造完，在Action.CreateASTConsumer方法中 调用mRewriter.setSourceMgr后即可正常使用
    mRewriter.setSourceMgr(SM, langOpts);



    //Rewriter:3:  Action将Rewriter传递给Consumer
    //Act中 是 每次都是 新创建 CTkAstCnsm
    return std::make_unique<CTkAstCnsm>(CI, mRewriter,
                                        &astContext, SM, langOpts);
  }


    void EndSourceFileAction() override {
//      mRewriter
//         .getEditBuffer(mRewriter.getSourceMgr().getMainFileID())
//         .write(llvm::outs());

//      mRewriter.overwriteChangedFiles();//修改会影响原始文件
    }
private:
    //Rewriter:0:  Rewriter总是作为Action类中的一个成员字段.
    //Rewriter:1:  Rewriter并不是上层传递下来的，而是自己在这构造的.
    Rewriter mRewriter;//这里是独立运行Act中的Rewriter，是源头，理应构造Rewriter.
};



int main(int Argc, const char **Argv) {
  Expected<tooling::CommonOptionsParser> eOptParser =
          tooling::CommonOptionsParser::create(Argc, Argv, CTkAloneCategory);
  if (auto E = eOptParser.takeError()) {
    errs() << "Problem constructing CommonOptionsParser "
           << toString(std::move(E)) << '\n';
    return EXIT_FAILURE;
  }

  // 获取编译配置
  clang::tooling::CompilationDatabase &Compilations = eOptParser->getCompilations();//你写的代码，这行报错：No member named 'getCompilations' in 'clang::tooling::ClangTool'


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


  // 设置文件名
  const char* FileName = "/pubx/clang-ctk/t_clock_tick/test_main.cpp";
  if(Argc>=2 && Argv[1]){
    //如果命令行 有指定源文件路径 则用命令行的
    FileName=Argv[1];
  }
  clang::FileID MainFileID = CI.getSourceManager().getOrCreateFileID(
          CI.getFileManager().getVirtualFile(FileName, /*Size=*/0, /*ModificationTime=*/0),
          clang::SrcMgr::C_User);

  // 将文件 ID 设置为主文件
  CI.getSourceManager().setMainFileID(MainFileID);

  // 创建 ClangTool 对象
  std::vector<std::string> Files{FileName};
  clang::tooling::ClangTool Tool(Compilations, Files);//报错： No member named 'getCompilations' in 'clang::CompilerInstance'


  CI.getDiagnosticOpts().ShowColors = true;

  Tool.appendArgumentsAdjuster( clang::tooling::getInsertArgumentAdjuster("--verbose"));
  //解决： 找不到 stddef.h
  Tool.appendArgumentsAdjuster( clang::tooling::getInsertArgumentAdjuster({"-resource-dir","/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/lib/clang/15.0.0"},tooling::ArgumentInsertPosition::END));
  /* stddef.h位于:
/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/lib/clang/15.0.0/include/stddef.h
   */

  // 运行 ClangTool
  int Result = Tool.run(clang::tooling::newFrontendActionFactory<CTkAloneAct>().get());

  return Result;
}

