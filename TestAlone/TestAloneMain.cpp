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


        Util::printSourceRangeSimple(CI,"zzz","",SourceRange(beginLoc,endLoc),true);
        Util::printStmt(CI.getASTContext(),CI,"subS","",subStmt, true);

        RangeHasMacroAstVst rv(CI,SourceRange(beginLoc, endLoc));
        std::string rvAdrr=fmt::format("{:x}",reinterpret_cast<uintptr_t>(&rv));
        std::cout<< rvAdrr <<":开始case" << k <<  std::endl;
        rv.TraverseStmt(swtStmt);
        std::cout<< rvAdrr << ":结束case" << k << ",hasMacro:" << rv.hasMacro <<  "\n\n";

        if(rv.hasMacro || rv.caseKSubStmtCnt==0){
          //如果此case内有宏，则不处理
          continue;
        }

        //否则 此case内无宏，则处理

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
      ASTContext &astContext = CI.getASTContext();

      CI.getDiagnostics().setSourceManager(&SM);

      mRewriter_ptr->setSourceMgr(SM, langOptions);


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
  //region


  SourceManager& SM=CI.getSourceManager();
  LangOptions &LO = CI.getLangOpts();
  Preprocessor &PP = CI.getPreprocessor();

  //region 添加诊断
  llvm::raw_ostream &OS = llvm::outs();
  DiagnosticOptions *diagnosticOptions = new clang::DiagnosticOptions();
  clang::TextDiagnosticPrinter *TextDiag = new TextDiagnosticPrinter(OS, diagnosticOptions);
  TextDiag->BeginSourceFile(LO,&PP);
//  TextDiag->EndSourceFile();
  CI.getDiagnostics().setClient(TextDiag);
  //end

  //region 创建自定义Action 即 自定义clang插件
  clang::FrontendAction* Action = new MyASTFrontendAction();
  //endregion

  //region 设置ResourceDir?  这一步不确定是否生效 或 是否写对了 或 是否必要 (应该是必须要的)?
  HeaderSearchOptions &HSO = CI.getHeaderSearchOpts();
  HSO.ResourceDir=="/llvm_release_home/clang+llvm-15.0.6-x86_64-linux-gnu-ubuntu-18.04/lib/clang/15.0.0";
  //endregion

  //region 添加输入源码文件
  CI.getFrontendOpts().Inputs.push_back(clang::FrontendInputFile("/pubx/clang-brc/test_in/test_main.cpp", clang::InputKind(clang::Language::CXX)));
  //endregion

  //region 运行自定义Action
  if (!CI.ExecuteAction(*Action)) {
    llvm::errs() << "Clang compilation failed\n";
    return 1;
  }
  //endregion

  return 0;
}