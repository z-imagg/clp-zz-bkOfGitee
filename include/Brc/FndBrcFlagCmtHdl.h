#ifndef FndBrcFlagCommentROVst_H
#define FndBrcFlagCommentROVst_H


#include <clang/Rewrite/Core/Rewriter.h>
#include <set>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Lex/Preprocessor.h>
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/Stmt.h"
#include "clang/Basic/SourceManager.h"

using namespace llvm;
using namespace clang;

/**
 * 只读注释遍历器,  找已插入括号的标记注释
 */
class FndBrcFlagCmtHdl
        : public CommentHandler {
public:
    static const std::string BrcInsrtFlagComment;
public:
    //Rewriter:4:  Consumer将Rewriter传递给Visitor
    explicit FndBrcFlagCmtHdl(CompilerInstance &_CI
//                                    , SourceManager &_SM, LangOptions &_langOptions
                                    )
    //Rewriter:5:  Consumer将Rewriter传递给Visitor, 并由Visitor.mRewriter接收
    :
            CI(_CI),
//            SM(_SM),
//            langOptions(_langOptions),
            curMainFileHas_BrcInsrtFlag(false)
    {

    }

    virtual bool HandleComment(Preprocessor &PP, SourceRange Comment);



public:
    CompilerInstance &CI;
//    SourceManager &SM;
//    LangOptions &langOptions;
    bool curMainFileHas_BrcInsrtFlag;


};


#endif