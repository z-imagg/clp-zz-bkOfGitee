#include "CTk/CTkAstCnsm.h"

#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Rewrite/Core/Rewriter.h"

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

  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 StringRef file) override {
    SourceManager& SM=CI.getSourceManager();
    LangOptions &langOpts=CI.getLangOpts();
    ASTContext& astContext=CI.getASTContext();
    //Rewriter:2:  Rewriter构造完，在Action.CreateASTConsumer方法中 调用mRewriter.setSourceMgr后即可正常使用
    mRewriter.setSourceMgr(SM, langOpts);

    //Rewriter:3:  Action将Rewriter传递给Consumer
    return std::make_unique<CTkAstCnsm>(CI, mRewriter,
                                        &astContext, SM, langOpts);
  }


    void EndSourceFileAction() override {
//      mRewriter
//         .getEditBuffer(mRewriter.getSourceMgr().getMainFileID())
//         .write(llvm::outs());

      mRewriter.overwriteChangedFiles();//修改会影响原始文件
    }
private:
    //Rewriter:0:  Rewriter总是作为Action类中的一个成员字段.
    //Rewriter:1:  Rewriter并不是上层传递下来的，而是自己在这构造的.
    Rewriter mRewriter;
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

  const std::unique_ptr<tooling::FrontendActionFactory> &FrontendFactory = tooling::newFrontendActionFactory<CTkAloneAct>();
  std::string Filename(Argv[1]);
  tooling::ToolAction *Action2=tooling::newFrontendActionFactory<CTkAloneAct>().get();
//  clang::tooling::ToolInvocation Invocation2(
//          {"-fsyntax-only", "-std=c++11"}, Action2 ,Filename);

//  return Tool.run(
//      tooling::newFrontendActionFactory<CTkAloneAct>().get());



/////////////////
  Expected<tooling::CommonOptionsParser> OptionsParser =
          tooling::CommonOptionsParser::create(Argc, Argv, CTkAloneCategory);
//  CommonOptionsParser OptionsParser(Argc, Argv, CTkAloneCategory);
  OptionsParser.get();

  // 获取输入文件列表
  std::vector<std::string> InputFiles = OptionsParser->getSourcePathList();

  // 创建FileManager和SourceManager
  clang::FileSystemOptions fileSystemOptions;
  FileManager FM(fileSystemOptions);
  DiagnosticsEngine *diagnosticsEngine = CompilerInstance::createDiagnostics(new DiagnosticOptions()).get();
  SourceManager SM(*diagnosticsEngine,FM);

  // 添加输入文件到FileManager
  for (const std::string &InputFile : InputFiles) {
    const FileEntry *&pFileEntry = FM.getFile(InputFile).get();
//    llvm::ErrorOr<llvm::vfs::pFileEntry> pFileEntry = llvm::vfs::pFileEntry(InputFile);
    if (!pFileEntry) {
      llvm::errs() << "Failed to get file status: " << InputFile << "\n";
      return 1;
    }

    // 打开文件
    llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>> FileBuffer =
            llvm::MemoryBuffer::getFile(InputFile);
    if (!FileBuffer) {
      llvm::errs() << "Failed to open file: " << InputFile << "\n";
      return 1;
    }

    // 在FileManager中创建文件
    const FileEntry *File =
            FM.getVirtualFile(InputFile, FileBuffer->get()->getBufferSize(), 0);
    if (!File) {
      llvm::errs() << "Failed to create file: " << InputFile << "\n";
      return 1;
    }

    // 在SourceManager中添加文件
    FileID FID = SM.createFileID(File, SourceLocation(), SrcMgr::C_User);
    SM.setMainFileID(FID);
  }

  // 创建编译器实例
  CompilerInstance CI;
  CI.createDiagnostics();

  // 设置FileManager和SourceManager
  CI.setFileManager(&FM);
  CI.setSourceManager(&SM);

  // 创建FrontendAction
//  const std::unique_ptr<tooling::FrontendActionFactory> &FrontendFactory = tooling::newFrontendActionFactory<CTkAloneAct>();
  tooling::FrontendActionFactory *Action = tooling::newFrontendActionFactory<CTkAloneAct>().get();
  const std::unique_ptr<tooling::FrontendActionFactory> &Action0 = tooling::newFrontendActionFactory<CTkAloneAct>();
  std::unique_ptr<FrontendAction> FrontendAction = std::make_unique<CTkAloneAct>();
//  FrontendAction *act = Action0->create().get();
//  const std::unique_ptr<FrontendAction> &act = Action0->create();
//  act = Action0;
//  std::unique_ptr<FrontendActionFactory> FrontendFactory =
//          newFrontendActionFactory<MyFrontendAction>();

  InputFiles.insert(InputFiles.begin(),"-fsyntax-only");
  // 创建ToolInvocation
  clang::tooling::ToolInvocation Invocation(InputFiles, std::move(FrontendAction) , &FM );

  // 运行ToolInvocation
  Invocation.run();

  return 0;
}
