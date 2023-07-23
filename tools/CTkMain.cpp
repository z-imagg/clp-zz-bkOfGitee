#include "CTk/CTkAstCnsm.h"

#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
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

//    CI.getPreprocessorOpts().addMacroDef("__internal_isystem=/usr/xxx/include");

/*
/app/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/bin/clang -v -c /pubx/clang-ctk/t_clock_tick/test_main.cpp
clang version 15.0.0 (git@github.com:llvm/llvm-project.git 4ba6a9c9f65bbc8bd06e3652cb20fd4dfc846137)
Target: x86_64-unknown-linux-gnu
Thread model: posix
InstalledDir: /app/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/bin
Found candidate GCC installation: /usr/lib/gcc/x86_64-linux-gnu/11
Selected GCC installation: /usr/lib/gcc/x86_64-linux-gnu/11
Candidate multilib: .;@m64
Candidate multilib: 32;@m32
Candidate multilib: x32;@mx32
Selected multilib: .;@m64
 (in-process)
 "/app/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/bin/clang-15" -cc1 -triple x86_64-unknown-linux-gnu -emit-obj -mrelax-all --mrelax-relocations -disable-free -clear-ast-before-backend -disable-llvm-verifier -discard-value-names -main-file-name test_main.cpp -mrelocation-model pic -pic-level 2 -pic-is-pie -mframe-pointer=all -fmath-errno -ffp-contract=on -fno-rounding-math -mconstructor-aliases -funwind-tables=2 -target-cpu x86-64 -tune-cpu generic -mllvm -treat-scalable-fixed-error-as-warning -debugger-tuning=gdb -v -fcoverage-compilation-dir=/ -resource-dir /app/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/lib/clang/15.0.0 -internal-isystem /usr/lib/gcc/x86_64-linux-gnu/11/../../../../include/c++/11 -internal-isystem /usr/lib/gcc/x86_64-linux-gnu/11/../../../../include/x86_64-linux-gnu/c++/11 -internal-isystem /usr/lib/gcc/x86_64-linux-gnu/11/../../../../include/c++/11/backward -internal-isystem /app/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/lib/clang/15.0.0/include -internal-isystem /usr/local/include -internal-isystem /usr/lib/gcc/x86_64-linux-gnu/11/../../../../x86_64-linux-gnu/include -internal-externc-isystem /usr/include/x86_64-linux-gnu -internal-externc-isystem /include -internal-externc-isystem /usr/include -fdeprecated-macro -fdebug-compilation-dir=/ -ferror-limit 19 -fgnuc-version=4.2.1 -fcxx-exceptions -fexceptions -fcolor-diagnostics -faddrsig -D__GCC_HAVE_DWARF2_CFI_ASM=1 -o test_main.o -x c++ /pubx/clang-ctk/t_clock_tick/test_main.cpp
clang -cc1 version 15.0.0 based upon LLVM 15.0.0 default target x86_64-unknown-linux-gnu
ignoring nonexistent directory "/usr/lib/gcc/x86_64-linux-gnu/11/../../../../x86_64-linux-gnu/include"
ignoring nonexistent directory "/include"
#include "..." search starts here:
#include <...> search starts here:
 /usr/lib/gcc/x86_64-linux-gnu/11/../../../../include/c++/11
 /usr/lib/gcc/x86_64-linux-gnu/11/../../../../include/x86_64-linux-gnu/c++/11
 /usr/lib/gcc/x86_64-linux-gnu/11/../../../../include/c++/11/backward
 /app/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/lib/clang/15.0.0/include
 /usr/local/include
 /usr/include/x86_64-linux-gnu
 /usr/include

 */
//    CI.getFrontendOpts().Verbosity = true;
//    CI.getDiagnosticOpts().setVerbose(true);

//    CI.getInvocation().getDiagnosticOpts().setDiagnosticOption( clang::DiagnosticIDs::DriverMode, "-verbose");
    CI.getDiagnosticOpts().ShowColors = true;



//    clang::PreprocessorOptions& preprocessorOptions=CI.getPreprocessorOpts();
//    preprocessorOptions.addMacroDef("__internal_isystem=/usr/lib/gcc/x86_64-linux-gnu/11/../../../../include/c++/11");
//    preprocessorOptions.addMacroDef("__internal_isystem=/usr/lib/gcc/x86_64-linux-gnu/11/../../../../include/x86_64-linux-gnu/c++/11");
//    preprocessorOptions.addMacroDef("__internal_isystem=/usr/lib/gcc/x86_64-linux-gnu/11/../../../../include/c++/11/backward");
//    preprocessorOptions.addMacroDef("__internal_isystem=/app/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/lib/clang/15.0.0/include");
//    preprocessorOptions.addMacroDef("__internal_isystem=/usr/local/include");
//    preprocessorOptions.addMacroDef("__internal_isystem=/usr/lib/gcc/x86_64-linux-gnu/11/../../../../x86_64-linux-gnu/include");
//
////    CI.getInvocation().getLangOpts()->InternalExternCIsystem = "/zzz";
////    CI.getInvocation().getLangOpts()->ExternCIsystem = "/zzz";
//    CI.getPreprocessorOpts().addMacroDef("__internal_externc_isystem=/usr/include/x86_64-linux-gnu");
//    CI.getPreprocessorOpts().addMacroDef("__internal_externc_isystem=/usr/include/x86_64-linux-gnu");
//    CI.getPreprocessorOpts().addMacroDef("__internal_externc_isystem=/include");
//    CI.getPreprocessorOpts().addMacroDef("__internal_externc_isystem=/usr/include");



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

//===----------------------------------------------------------------------===//
// Main driver code.
//===----------------------------------------------------------------------===//
int main(int Argc, const char **Argv) {
  Expected<tooling::CommonOptionsParser> eOptParser =
      tooling::CommonOptionsParser::create(Argc, Argv, CTkAloneCategory);
  if (auto E = eOptParser.takeError()) {
    errs() << "Problem constructing CommonOptionsParser "
           << toString(std::move(E)) << '\n';
    return EXIT_FAILURE;
  }
  tooling::ClangTool Tool(eOptParser->getCompilations(),
                                 eOptParser->getSourcePathList());


  Tool.appendArgumentsAdjuster(
          clang::tooling::getInsertArgumentAdjuster("-I/usr/lib/gcc/x86_64-linux-gnu/11/include"));

  return Tool.run(
      tooling::newFrontendActionFactory<CTkAloneAct>().get());
}
