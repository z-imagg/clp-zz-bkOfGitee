#ifndef CTkVst_H
#define CTkVst_H


#include <clang/Rewrite/Core/Rewriter.h>
#include <set>
#include <clang/Frontend/CompilerInstance.h>
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/Stmt.h"
#include "clang/Basic/SourceManager.h"

using namespace llvm;
using namespace clang;

//-----------------------------------------------------------------------------
// RecursiveASTVisitor
//-----------------------------------------------------------------------------
class CTkVst
        : public RecursiveASTVisitor<CTkVst> {
public:
    //Rewriter:4:  Consumer将Rewriter传递给Visitor
    explicit CTkVst(Rewriter &R, ASTContext *Ctx, CompilerInstance &CI, SourceManager& SM)
    //Rewriter:5:  Consumer将Rewriter传递给Visitor, 并由Visitor.mRewriter接收
    : mRewriter(R),
    Ctx(Ctx),
    CI(CI),
    SM(SM)
    {

    }

    static const std::string funcName_TCTk ;//= "X__t_clock_tick";
    static const std::string IncludeStmt_TCTk ; // = "#include \"t_clock_tick.h\"\n";

    /**遍历语句
     *
     * @param stmt
     * @return
     */
//    bool VisitStmt(Stmt *S) { return true; } : grep '(Stmt'  /llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/include/clang/AST/RecursiveASTVisitor.h
    bool processStmt(Stmt *stmt,const char* whoInserted=NULL);
//    DEF_TRAVERSE_STMT(CallExpr      : grep '(CallExpr'  /llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/include/clang/AST/RecursiveASTVisitor.h
//    virtual bool VisitCallExpr(CallExpr *callExpr);

    //DEF_TRAVERSE_STMT(CompoundStmt  : grep '(CompoundStmt'  /llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/include/clang/AST/RecursiveASTVisitor.h
    virtual bool TraverseCompoundStmt(CompoundStmt *compoundStmt );
    virtual bool TraverseIfStmt(IfStmt *ifStmt);
    virtual bool TraverseWhileStmt(WhileStmt *whileStmt);
    virtual bool TraverseForStmt(ForStmt *forStmt);
    virtual bool TraverseCXXTryStmt(CXXTryStmt *cxxTryStmt);
    virtual bool TraverseCXXCatchStmt(CXXCatchStmt *cxxCatchStmt);
    virtual bool TraverseDoStmt(DoStmt *doStmt);
    virtual bool TraverseSwitchStmt(SwitchStmt *switchStmt);
//    virtual bool TraverseCaseStmt(CaseStmt *caseStmt);由于case语句前不能插入 任何语句 ,否则语法错误，因此 case语句不需要自定义处理，只需要对case语句用clang内部的正常递归即可。
    //这里应该有 所有能带块的语句: if块、while块、else块、for块、switch块、try块、catch块...

    /////////constexpr
    virtual bool TraverseFunctionDecl(FunctionDecl* functionDecl);
    virtual bool TraverseCXXMethodDecl(CXXMethodDecl* cxxMethodDecl);

    /** 遍历  FunctionDecl 或 CXXMethodDecl
     * TraverseFunctionDecl 和 TraverseCXXMethodDecl 的 公共代码
     * @param funcSourceRange
     * @param funcIsConstexpr
     * @param funcBodyStmt
     * @return
     */
    bool _Traverse_Func(
            const SourceRange &funcSourceRange,
            bool funcIsConstexpr,
            Stmt *funcBodyStmt
    );



  /**
   * 返回 该坐标Loc 是否 在 任意一个 被 constexpr 修饰的 函数 的坐标范围内, 若在 ，则 此坐标 不能被插入语句。
   * @param Loc
   * @return
   */
    bool LocIsIn_constexpr_func_ls(SourceLocation Loc);
public:
    //这里是Visitor中的Rewriter，非源头，不要构造Rewriter，只能引用Act中的源头Rewriter.
    // 若这里也构造，将出现两个Rewriter, 则后一个Rewriter写入时会覆盖前一个Rewriter的写入，导致前一个Rewriter的写入丢失。
    //     这里前一个是命名空间中的函数们，后一个是顶层函数们。
    //     即 看起来的现象： 命名空间中的函数们 时钟调用语句 丢失， 而顶层函数们写入的时中调用语句 还在。
    Rewriter & mRewriter;

public:
    ASTContext *Ctx;
    CompilerInstance& CI;
    SourceManager& SM;

    /** 存放有constexpr修饰的函数整体坐标范围
     * 方便 processStmt 判断 当前语句 是否在这些函数坐标范围内，若是 则不插入语句
     */
    std::list<SourceRange> constexpr_func_ls;

    /**其前已经插入语句的 节点ID 们,
     * 为防止重复遍历导致的重复插入，
     * 可达到： 即使重复遍历了 但不会重复插入
     * 如果后面发现ID 不是全局唯一的 可以尝试换成 该节点的开始位置
     */
    std::set<int64_t> insertedNodeIDLs;



};


#endif