
#ifndef FuncDesc_H
#define FuncDesc_H



#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/Stmt.h"


using namespace llvm;
using namespace clang;

struct FuncDesc{
public:
    QualType funcReturnType;
    bool isaCXXConstructorDecl;
    Stmt *endStmtOfFuncBody;
    SourceLocation funcBodyRBraceLoc;
public:
    FuncDesc(QualType funcReturnType,
             bool isaCXXConstructorDecl,
             Stmt *endStmtOfFuncBody,
             SourceLocation funcBodyRBraceLoc);
};
#endif //FuncDesc_H
