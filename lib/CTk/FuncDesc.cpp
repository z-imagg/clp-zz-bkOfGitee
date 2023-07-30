



#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/Stmt.h"
#include "CTk/FuncDesc.h"


using namespace llvm;
using namespace clang;

FuncDesc::FuncDesc(QualType funcReturnType,
             bool isaCXXConstructorDecl,
             Stmt *endStmtOfFuncBody,
             SourceLocation funcBodyRBraceLoc)
            :
            funcReturnType(funcReturnType),
            isaCXXConstructorDecl(isaCXXConstructorDecl),
            endStmtOfFuncBody(endStmtOfFuncBody),
            funcBodyRBraceLoc(funcBodyRBraceLoc)
    {

    }
