
#ifndef RangeHasMacroAstVst_H
#define RangeHasMacroAstVst_H


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


class RangeHasMacroAstVst : public RecursiveASTVisitor<RangeHasMacroAstVst> {
public:
    CompilerInstance& CI;
    SourceRange caseKSrcRange;
    bool hasMacro;
    //caseK的子语句个数
    int caseKSubStmtCnt;

    explicit RangeHasMacroAstVst(CompilerInstance &_CI , SourceRange _sourceRange )
            :
            CI(_CI),
            caseKSrcRange(_sourceRange),
            hasMacro(false),
            caseKSubStmtCnt(0)
    {

    }
    bool VisitStmt(clang::Stmt *stmt) ;
};


#endif //RangeHasMacroAstVst_H
