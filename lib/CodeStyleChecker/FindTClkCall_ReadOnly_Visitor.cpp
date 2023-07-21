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
    //取函数名字， funcName  是  "X__t_clock_tick"
//    std::string funcName = dirtCallee->getNameInfo().getName().getAsString();
    std::string funcName = dirtCallee->getName().str();
    bool isTCTickCall= (funcName==CodeStyleCheckerVisitor::funcName_TCTick);
    //记录 该函数 是否 时钟调用语句
//    bool isStartWithTCTickCall=funcName.rfind( CodeStyleCheckerVisitor::funcName_TCTick,0)==0;
    if(isTCTickCall){
      //注意，找到时钟调用语句时 才记录标志。如果每次都记录，标记会被其他函数调用覆盖掉。
      this->curMainFileHas_TCTickCall=isTCTickCall;
    }

    //{开发打日志
    {
    std::string fileAndRange = sourceRange.printToString(SM);
    std::cout << "调用语句： 在文件位置:" << fileAndRange << ",调用语句" << sourceText << std::endl;
    if (isTCTickCall) {
      std::cout << "发现时钟调用语句： 在文件位置:" << fileAndRange << ",调用语句" << sourceText << std::endl;
//      return false;//注意：返回false并不能结束上层的循环调用自己
    }
    }
    //}

  }

  return true;
}





