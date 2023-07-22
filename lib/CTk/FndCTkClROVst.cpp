#include "CTk/FndCTkClROVst.h"
#include "CTk/CTkVst.h"
#include "clang/AST/AST.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "CTk/Util.h"

using namespace llvm;
using namespace clang;

#include <iostream>
#include <clang/AST/ParentMapContext.h>



bool FndCTkClROVst::VisitCallExpr(CallExpr *callExpr){
  //{开发用
  SourceRange sourceRange = callExpr->getSourceRange();
  std::string fileAndRange = sourceRange.printToString(SM);
  std::string sourceText = Util::getSourceTextBySourceRange(sourceRange, SM, langOptions);
  //}

//  std::string fn=callExpr->getDirectCallee()->getNameInfo().getName().getAsString(); //备忘

  FunctionDecl* dirtCallee=callExpr->getDirectCallee();
  if(dirtCallee==NULL){
//    std::cout << "学习用,callExpr->getDirectCallee()==NULL： 在文件位置:" << fileAndRange << ",调用语句" << sourceText << std::endl;
//Util::printExpr(CI,)
    return true;
  }

  IdentifierInfo *identifierInfo = dirtCallee->getIdentifier();
  if(identifierInfo==NULL){
    //sourceText 可以 为 ++, 此时 identifierInfo 为NULL， 即 这是 操作符函数，自然是没有函数名字的，此时 dirtCallee->getName() 一定会出错。忽略即可。
//    std::cout << "学习用,callExpr->getDirectCallee()->getIdentifier()==NULL： 在文件位置:" << fileAndRange << ",调用语句" << sourceText << std::endl;
    return true;
  }

    std::string funcName = dirtCallee->getName().str();
    bool isTCTkCall= (funcName==CTkVst::funcName_TCTk);
    //记录 该函数调用 是否 时钟调用语句
    if(isTCTkCall){

      //{开发打日志
      if (!this->curMainFileHas_TCTkCall) {//只在第一次找到 时钟调用语句 时，打印日志
        std::cout << "此文件已处理,发现时钟调用语句： 在文件位置:" << fileAndRange << ",调用语句" << sourceText << std::endl;
      }
      //}

      //注意，找到时钟调用语句时 才记录标志。如果每次都记录，标记会被其他函数调用覆盖掉。
      this->curMainFileHas_TCTkCall=isTCTkCall;
    }


  return true;
}





