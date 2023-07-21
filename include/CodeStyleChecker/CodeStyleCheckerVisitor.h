
#include <clang/Rewrite/Core/Rewriter.h>
#include <set>
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/Stmt.h"
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

    static const std::string funcName_TCTick ;//= "X__t_clock_tick";
    static const std::string IncludeStmt_TCTick ; // = "#include \"t_clock_tick.h\"\n";
    static std::set<clang::FileID> fileInsertedIncludeStmt;
    static void insertIncludeToFileStart(clang::FileID fileId, clang::SourceManager &SM, clang::Rewriter& rewriter);
    static void insertIncludeToFileStartByLoc(clang::SourceLocation Loc, clang::SourceManager &SM, clang::Rewriter& rewriter);
    static bool getSourceFilePathAtLoc(clang::SourceLocation Loc, const clang::SourceManager &SM,clang::StringRef& fn);
    static bool getSourceFilePathOfStmt(const clang::Stmt *S, const clang::SourceManager &SM,clang::StringRef& fn);

    static clang::FunctionDecl* findFuncDecByName(clang::ASTContext *Ctx,std::string functionName);
    static std::string getSourceTextBySourceRange(clang::SourceRange sourceRange, clang::SourceManager & sourceManager, const clang::LangOptions & langOptions);

    /**遍历语句
     *
     * @param stmt
     * @return
     */
    virtual bool VisitStmt(clang::Stmt *stmt);
    virtual bool VisitCallExpr(clang::CallExpr *callExpr);

    //下面4个形如 bool VisitZzz(clang:Zzz *Decl)  的方法:, 其中Zzz的完整列表叙述 从 本文件第27行开始.
    bool VisitCXXRecordDecl(clang::CXXRecordDecl *Decl);
    bool VisitFunctionDecl(clang::FunctionDecl *Decl);
    bool VisitVarDecl(clang::VarDecl *Decl);
    bool VisitFieldDecl(clang::FieldDecl *Decl);


public:
    clang::Rewriter mRewriter;

private:
    clang::ASTContext *Ctx;

};
