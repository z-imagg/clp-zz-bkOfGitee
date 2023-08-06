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

using namespace clang;


class RangeHasMacroAstVisitor : public RecursiveASTVisitor<RangeHasMacroAstVisitor> {
public:
    CompilerInstance& CI;
    SourceRange caseKSrcRange;
    bool hasMacro;

    explicit RangeHasMacroAstVisitor(CompilerInstance &_CI , SourceRange _sourceRange )
    :
            CI(_CI),
            caseKSrcRange(_sourceRange),
            hasMacro(false)
    {

    }
    bool VisitStmt(clang::Stmt *stmt) {
      SourceLocation CB = caseKSrcRange.getBegin();
      SourceLocation CE = caseKSrcRange.getEnd();


      SourceManager &SM = CI.getSourceManager();
      int caseKBL,caseKBC;
      Util::extractLineAndColumn(SM,CB,caseKBL,caseKBC);
      int caseKEL,caseKEC;
      Util::extractLineAndColumn(SM,CE,caseKEL,caseKEC);



      SourceLocation B = stmt->getBeginLoc();
      SourceLocation E = stmt->getEndLoc();
      int sBL,sBC;
      Util::extractLineAndColumn(SM,B,sBL,sBC);
      int sEL,sEC;
      Util::extractLineAndColumn(SM,E,sEL,sEC);

      //如果已经确定 给定Switch的限定位置范围内 有宏，则直接返回，且不需要再遍历了
      if(hasMacro){
        return false;
      }


      //如果遍历到块语句，不处理，直接返回。因为这里只关心单语句，不关心块语句。
      if (clang::isa<clang::CompoundStmt>(stmt)) {
        return true;
      }


      //如果遍历到的语句的返回 不完全 含在 限定位置范围内 ，不处理，直接返回。 这种情况可能是拿到了一个更大的非终结符号。
//      bool FC = caseKSrcRange.fullyContains(stmt->getSourceRange());
      bool FC = CB<=B && CE>=E;
      bool FC_=(caseKBL<=sBL) && (caseKEL>=sEL);
      if(!FC_){//不要用FC 否则结果是错的
        return true;///
      }


      std::string rvAdrr=fmt::format("{:x}",reinterpret_cast<uintptr_t>(this));
      Util::printStmt(CI.getASTContext(),CI,"xxx",rvAdrr,stmt,true);


      //如果遍历到的单语句，开始位置在宏中 或 结束位置在宏中，则 给定Switch的限定位置范围内 有宏，直接返回，且不需要再遍历了。
      bool inMacro = Util::LocIsInMacro(B,SM) || Util::LocIsInMacro(E,SM);

      if(!hasMacro ){
        if(inMacro){
          hasMacro=true;
          return false;
        }
      }

      //其他情况，继续遍历
      return true;
    }
};


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


        RangeHasMacroAstVisitor rv(CI,SourceRange(beginLoc, endLoc));
        std::string rvAdrr=fmt::format("{:x}",reinterpret_cast<uintptr_t>(&rv));
        std::cout<< rvAdrr <<":开始case" << k <<  std::endl;
        rv.TraverseStmt(swtStmt);
        std::cout<< rvAdrr << ":结束case" << k << ",hasMacro:" << rv.hasMacro <<  "\n\n";
        Util::printSourceRangeSimple(CI,"","",SourceRange(beginLoc,endLoc),true);

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
  clang::FrontendAction* Action = new MyASTFrontendAction();

  HeaderSearchOptions &HSO = CI.getHeaderSearchOpts();
  HSO.ResourceDir=="/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/lib/clang/15.0.0";

  // 设置输入文件
  CI.getFrontendOpts().Inputs.push_back(clang::FrontendInputFile("/pubx/clang-brc/test_in/test_main.cpp", clang::InputKind(clang::Language::CXX)));

//  HSO
  // 运行 Clang 编译
  if (!CI.ExecuteAction(*Action)) {
    llvm::errs() << "Clang compilation failed\n";
    return 1;
  }

  return 0;
}