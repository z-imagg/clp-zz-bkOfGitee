
#include <clang/Rewrite/Core/Rewriter.h>
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Basic/SourceManager.h"

//-----------------------------------------------------------------------------
// RecursiveASTVisitor
//-----------------------------------------------------------------------------
class CodeStyleCheckerVisitor
        : public clang::RecursiveASTVisitor<CodeStyleCheckerVisitor> {
public:
    //Rewriter:4:  Consumer将Rewriter传递给Visitor
    explicit CodeStyleCheckerVisitor(clang::Rewriter &R, clang::ASTContext *Ctx)
    //Rewriter:5:  Consumer将Rewriter传递给Visitor, 并由Visitor.mRewriter接收
    : mRewriter(R),
    Ctx(Ctx)
    {

    }
    bool VisitCXXRecordDecl(clang::CXXRecordDecl *Decl);
    bool VisitFunctionDecl(clang::FunctionDecl *Decl);
    bool VisitVarDecl(clang::VarDecl *Decl);
    bool VisitFieldDecl(clang::FieldDecl *Decl);

public:
    clang::Rewriter mRewriter;

private:
    clang::ASTContext *Ctx;

    // Checks whether the name in Decl contains any `_`. Issues a warnning if it
    // does.
    void checkNoUnderscoreInName(clang::NamedDecl *Decl);
    // Checks whether the name in Decl starts with a lower case letter. Issues a
    // warning if not.
    void checkNameStartsWithLowerCase(clang::NamedDecl *Decl);
    // Checks whether the name in Decl starts with an upper case letter. Issues a
    // warning if not.
    void checkNameStartsWithUpperCase(clang::NamedDecl *Decl);
};
