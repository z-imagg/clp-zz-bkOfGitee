//
// Created by zz on 2023/8/2.
//

#ifndef CmtVst_H
#define CmtVst_H

#include "Brc/FndBrcFlagCmtHdl.h"
#include "Util.h"
#include "Brc/BrcVst.h"
#include <fmt/core.h>
#include "clang/AST/CommentVisitor.h"
#include "clang/AST/Comment.h"
#include "clang/Basic/SourceManager.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/AST/ASTConsumer.h"
#include <clang/Frontend/CompilerInstance.h>
#include <filesystem>
#include <iostream>
#include <clang/Rewrite/Core/Rewriter.h>

class CmtVst : public comments::CommentVisitor<CmtVst> {
public:
    bool flag;
    std::string brcInsertedOK;//TODO_ 改为 static
    CompilerInstance &CI;
    ASTContext *Ctx;
    SourceManager &SM;
public:
    CmtVst(CompilerInstance &_CI,  ASTContext *_astContext,
           SourceManager &_SM)
    :
            flag(false),
            brcInsertedOK("/**BrcInsertedOk*/"),
            CI(_CI), Ctx(_astContext), SM(_SM)
//    comments::CommentVisitor<CmtVst>(*this)
    {

    }


    void visitTextComment(const comments::TextComment *C) {
      // 处理文本注释
      outs() << "visitTextComment:" << C->getText() << "\n";
    }

    void visitBlockCommandComment(const comments::BlockCommandComment *C) {
      // 处理块命令注释
      outs() <<  "visitBlockCommandComment:"  << C  << "\n";
    }

    void visitComment(const comments::Comment *C) {
      std::cout<< "visitComment:"  << C << std::endl;
      // 处理注释
      // ...
    }
    //刚才访问不到的原因是 方法名写错了 不是 VisitFullComment ，是 visitFullComment，第一个字母是小写v
    void visitFullComment(const comments::FullComment *C) {
//      std::cout << "visitFullComment:" << C << std::endl;

      if(flag){
        return;
      }

      const SourceRange &sourceRange = C->getSourceRange();
      LangOptions &langOpts = CI.getLangOpts();
      std::string sourceText = Util::getSourceTextBySourceRange(sourceRange, SM, langOpts);
//      SM.isWrittenInBuiltinFile(C->getLocation());
//      SM.isWrittenInMainFile(C->getLocation());
//      SM.isWrittenInSameFile(Loc1,Loc2);
//      SM.isInSystemMacro(Loc);
//      SM.isMacroArgExpansion(Loc);
//      SM.isMacroBodyExpansion(Loc);
      flag= (sourceText==brcInsertedOK);

    }

};

#endif //CmtVst_H
