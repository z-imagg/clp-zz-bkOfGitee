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

#include "base/Util.h"
#include "Brc/RangeHasMacroAstVst.h"
#include "Brc/CollectIncMacro_PPCb.h"


using namespace clang;

class MyASTVisitor : public RecursiveASTVisitor<MyASTVisitor> {
public:
    CompilerInstance& CI;

    explicit MyASTVisitor(CompilerInstance &_CI, const std::shared_ptr<Rewriter> _rewriter_ptr ) : CI(_CI)  {

    }

    virtual bool TraverseSwitchStmt(SwitchStmt *swtStmt){
      SourceManager &SM = CI.getSourceManager();

      SwitchCase *caseList = swtStmt->getSwitchCaseList();
      LangOptions &LO = CI.getLangOpts();

      std::vector<SwitchCase*> caseVec;
      for (SwitchCase* switchCase = caseList; switchCase; switchCase = switchCase->getNextSwitchCase()) {
        if(Util::LocIsInMacro(switchCase->getBeginLoc(),SM)){//如果此case是宏展开后产物，则跳过
          continue;
        }
        caseVec.push_back(switchCase);
      }

      std::sort(caseVec.begin(), caseVec.end(), [](clang::SwitchCase* lhs, clang::SwitchCase* rhs) {
          return lhs->getBeginLoc() < rhs->getBeginLoc();
      });


      size_t caseCnt = caseVec.size();
      for(int k=0; k < caseCnt; k++) {
        SwitchCase *sCaseK = caseVec[k];
        Stmt *subStmt = sCaseK->getSubStmt();
        const Stmt::child_range &child = sCaseK->children();
        size_t childSize = std::distance(child.begin(), child.end());
        
        SourceLocation beginLoc;
        SourceLocation endLoc;
        beginLoc = sCaseK->getColonLoc();
        if(k<caseCnt-1){
          endLoc=caseVec[k+1]->getBeginLoc();
        }else{
          endLoc=swtStmt->getEndLoc();
        }

        if ( isa<CaseStmt>(*sCaseK)) {
          CaseStmt *caseK = dyn_cast<CaseStmt>(sCaseK);

        }else if ( isa<DefaultStmt>(*sCaseK)) {
          DefaultStmt *defaultK = dyn_cast<DefaultStmt>(sCaseK);
        }


        SourceRange BE = SourceRange(beginLoc, endLoc);
        RangeHasMacroAstVst rv(CI,BE);
//        std::string rvAdrr=fmt::format("{:x}",reinterpret_cast<uintptr_t>(&rv));
//        std::cout<< rvAdrr <<":开始case" << k <<  std::endl;
//        Util::printSourceRangeSimple(CI,"zzz","",SourceRange(beginLoc,endLoc),true);
//        Util::printStmt(CI.getASTContext(),CI,"subS","",subStmt, true);
        rv.TraverseStmt(swtStmt);
//        std::cout<< rvAdrr << ":结束case" << k << ",hasMacro:" << rv.hasMacro <<  "\n\n";

        if(
    //如果此case内有宏，则不处理
    rv.hasMacro ||
    //如果此case内无子语句，则不处理
    rv.caseKSubStmtCnt==0 ||
    //预处理回调已经收集了#include、#define ，这里判断case起止范围内 有无#i、#d，若有 则不处理该case
     CollectIncMacro_PPCb::hasInclusionDirective(SM, BE) || CollectIncMacro_PPCb::hasMacroDefined(SM, BE)
        ){
//          Util::printSourceRangeSimple(CI,fmt::format("忽略case{}",k),"",BE,true);
          continue;
        }

        //否则处理此case
        Util::printSourceRangeSimple(CI,fmt::format("处理case{}",k),"",BE,true);
      }

      return true;
    }
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
      Preprocessor &PP = CI.getPreprocessor();
      ASTContext &astContext = CI.getASTContext();

      CI.getDiagnostics().setSourceManager(&SM);

      mRewriter_ptr->setSourceMgr(SM, langOptions);

      // Act中 添加 收集#include、#define的 预处理回调
      PP.addPPCallbacks(std::make_unique<CollectIncMacro_PPCb>(CI));

      
      return std::make_unique<MyASTConsumer>(CI,mRewriter_ptr);
    }
};

int main() {
  //region 创建编译器实例
  clang::CompilerInstance CI;
  //endregion

  //region 创建 诊断器、文件管理器、源码管理器
  CI.createDiagnostics();
  CI.createFileManager();
  CI.createSourceManager(CI.getFileManager());
  //endregion

  //region 设置语言为C++
  CI.getLangOpts().CPlusPlus = true;
  //endregion

  //region 设置目标平台
  CI.getTargetOpts().Triple= "x86_64-pc-linux-gnu";

  llvm::Triple triple("x86_64-pc-linux-gnu");
  std::shared_ptr<clang::TargetOptions> targetOpts=std::make_shared<clang::TargetOptions>();
  targetOpts->Triple=triple.str();
  TargetInfo* targetInfo=  TargetInfo::CreateTargetInfo(CI.getDiagnostics(), targetOpts) ;
  CI.setTarget(targetInfo);
  //endregion

  //region 创建预处理器、用目标平台初始化预处理器
  CI.createPreprocessor(clang::TU_Complete);
  CI.getPreprocessor().Initialize(*targetInfo);
  //endregion

  //region 为方便 而有 变量 SM、LO、PP
  SourceManager& SM=CI.getSourceManager();
  LangOptions &LO = CI.getLangOpts();
  Preprocessor &PP = CI.getPreprocessor();
  //endregion


  //region 添加诊断
  llvm::raw_ostream &OS = llvm::outs();
  DiagnosticOptions *diagnosticOptions = new clang::DiagnosticOptions();
  clang::TextDiagnosticPrinter *TextDiag = new TextDiagnosticPrinter(OS, diagnosticOptions);
  TextDiag->BeginSourceFile(LO,&PP);
  CI.getDiagnostics().setClient(TextDiag);
  //endregion

  //region 创建自定义Action 即 自定义clang插件
  clang::FrontendAction* Action = new MyASTFrontendAction();
  //endregion

  //region 设置头文件路径
  HeaderSearchOptions &HSO = CI.getHeaderSearchOpts();
  HSO.AddPath("/usr/include/",clang::frontend::Angled, false, false);
  HSO.AddPath("/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/lib/clang/15.0.0/include/",clang::frontend::Angled, false, false);
  HSO.AddPath("/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/include/",clang::frontend::Angled, false, false);
  //ResourceDir 应该是有用的，但是不知道如何正确添加
//  HSO.ResourceDir=="/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/lib/clang/15.0.0";
  //endregion

//  PP.EnterSourceFile()

  //region 添加输入源码文件
  FrontendInputFile srcFile = clang::FrontendInputFile("/pubx/clang-brc/test_in/test_main.cpp", clang::InputKind(clang::Language::CXX));
  CI.getFrontendOpts().Inputs.push_back(srcFile);
  //endregion

  //region 运行自定义Action
  if (!CI.ExecuteAction(*Action)) {
    llvm::errs() << "Clang compilation failed\n";
    return 1;
  }
  //endregion

  return 0;
}