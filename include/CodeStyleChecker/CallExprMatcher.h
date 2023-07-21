#ifndef CLANG_TUTOR_CALLEXPRMATCHER_H
#define CLANG_TUTOR_CALLEXPRMATCHER_H

#include "CodeStyleChecker/CodeStyleCheckerVisitor.h"
#include "clang/Basic/SourceManager.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/AST/ASTConsumer.h"
#include <iostream>
#include <clang/Rewrite/Core/Rewriter.h>

/**
 * generate by :  https://chat.chatgptdemo.net/
 */
class CallExprMatcher : public clang::ast_matchers::MatchFinder::MatchCallback {
public:
    clang::FileID startFileId;
    CallExprMatcher(clang::FileID _startFileId):startFileId(_startFileId) {}

    void run(const clang::ast_matchers::MatchFinder::MatchResult &Result) override {
    clang::SourceManager& SM=Result.Context->getSourceManager();
    if (const clang::CallExpr *CE = Result.Nodes.getNodeAs<clang::CallExpr>("callExpr")) {
      const clang::FunctionDecl* callee=CE->getDirectCallee();
      if(!callee){
        return;
      }

      clang::SourceLocation beginLoc=CE->getBeginLoc();
      clang::FileID fileId = SM.getFileID(beginLoc);

      std::string funcName=callee->getNameInfo().getName().getAsString();
      //能找到 时钟滴答 函数调用
      //若已经有时钟函数调用
      if(funcName==CodeStyleCheckerVisitor::funcName_TCTick
      //是否已处理 的 糟糕实现：匹配到CallExpr后看其FIleId时不是MainFileId, 若是 即为直接在本文件中，若不是则在间接包含的文件中
      && fileId==startFileId){
        //若已经有时钟函数调用，则标记为已处理
        CodeStyleCheckerVisitor::fileInsertedIncludeStmt.insert(fileId);
      }
      /*llvm::outs() << "Matched CallExpr at "
                   << CE->getBeginLoc().printToString(SM)
                   << "\n";*/
    }
  }

};


#endif //CLANG_TUTOR_CALLEXPRMATCHER_H
