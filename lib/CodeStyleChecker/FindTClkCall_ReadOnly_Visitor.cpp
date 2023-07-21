#include "CodeStyleChecker/FindTClkCall_ReadOnly_Visitor.h"
#include "CodeStyleChecker/CodeStyleCheckerVisitor.h"
#include "clang/AST/AST.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"

using namespace clang;

#include <iostream>
#include <clang/AST/ParentMapContext.h>



bool FindTClkCall_ReadOnly_Visitor::VisitCallExpr(clang::CallExpr *callExpr){
  //{开发用
  clang::SourceRange sourceRange = callExpr->getSourceRange();
  std::string sourceText = CodeStyleCheckerVisitor::getSourceTextBySourceRange(sourceRange, SM, langOptions);
  //}

//  std::string fn=callExpr->getDirectCallee()->getNameInfo().getName().getAsString(); //备忘

  FunctionDecl* dirtCallee=callExpr->getDirectCallee();
  if(dirtCallee){
    std::string funcName = dirtCallee->getName().str();
    bool isTCTickCall= (funcName==CodeStyleCheckerVisitor::funcName_TCTick);
    //记录 该函数调用 是否 时钟调用语句
//    bool isStartWithTCTickCall=funcName.rfind( CodeStyleCheckerVisitor::funcName_TCTick,0)==0;
    if(isTCTickCall){

      //{开发打日志
      if (!this->curMainFileHas_TCTickCall) {//只在第一次找到 时钟调用语句 时，打印日志
        std::string fileAndRange = sourceRange.printToString(SM);
        std::cout << "此文件已处理,发现时钟调用语句： 在文件位置:" << fileAndRange << ",调用语句" << sourceText << std::endl;
      }
      //}

      //注意，找到时钟调用语句时 才记录标志。如果每次都记录，标记会被其他函数调用覆盖掉。
      this->curMainFileHas_TCTickCall=isTCTickCall;
    }


  }

  return true;
}





