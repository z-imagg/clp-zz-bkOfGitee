#include "Brc/FndBrcFlagCmtHdl.h"
#include "Brc/BrcVst.h"
#include "clang/AST/AST.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "Brc/Util.h"

using namespace llvm;
using namespace clang;

#include <iostream>
#include <clang/AST/ParentMapContext.h>

const std::string FndBrcFlagCmtHdl::BrcInsrtFlagComment="/*__BrcInsertedOk*/";
bool FndBrcFlagCmtHdl::HandleComment(Preprocessor &PP, SourceRange commentSourceRange){
  SourceManager &SM=PP.getSourceManager();
  const LangOptions &langOptions=PP.getLangOpts();


  const std::string &comment = Util::getSourceTextBySourceRange(commentSourceRange, SM, langOptions);  //SyntaxOnlyAction在这里导致死循环

    bool isBraceInsertedFlag= (comment == FndBrcFlagCmtHdl::BrcInsrtFlagComment);
    //记录 该函数调用 是否 时钟调用语句
    if(isBraceInsertedFlag){

      //{开发打日志
      if (!this->curMainFileHas_BrcInsrtFlag) {//只在第一次找到 花括号已插入标记 时，打印日志
        Util::printSourceRangeSimple(CI, "查看注释", "此文件已处理,发现花括号已插入标记", commentSourceRange, true);
      }
      //}

      //注意，找到花括号已插入标记时 才记录标志。如果每次都记录，标记会被其他函数调用覆盖掉。
      this->curMainFileHas_BrcInsrtFlag=isBraceInsertedFlag;
    }

  return true;
}





