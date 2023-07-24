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
    /**
     * 栈变量 或 堆对象 的 生命步骤（生命阶段）
     */
    enum LifeStep{
        Alloc,
        Free
    };
public:
    //Rewriter:4:  Consumer将Rewriter传递给Visitor
    explicit CTkVst(const std::shared_ptr<Rewriter> rewriter_ptr, ASTContext *Ctx, CompilerInstance &CI, SourceManager& SM)
    //Rewriter:5:  Consumer将Rewriter传递给Visitor, 并由Visitor.mRewriter接收
    : mRewriter_ptr(rewriter_ptr),
    Ctx(Ctx),
    CI(CI),
    SM(SM)
    {

    }

    static const std::string funcName_TCTk ;//= "X__t_clock_tick";
    static const std::string IncludeStmt_TCTk ; // = "#include \"t_clock_tick.h\"\n";

    static bool isInternalSysSourceFile(StringRef fn);

    void insertBefore_X__t_clock_tick(LifeStep lifeStep, int64_t stmtId, SourceLocation stmtBeginLoc, int stackVarAllocCnt, int stackVarFreeCnt, int heapObjAllocCnt, int heapObjcFreeCnt, const char* whoInserted=NULL);

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
    /**
     * 1. if语句整体 前 插入 时钟调用语句
     * 2. 粘接直接子节点到递归链
     * 不直接处理if语句内的子语句
     *
     * 当if语句的子语句 child:[then, else]中 只有是块语句的情况下, 子语句内才需要 插入 时钟调用语句, 这是经转交 TraverseStmt(child) ---...---> TraverseCompoundStmt(child) 后，由TraverseCompoundStmt(child)对该块语句child中的每条语句前 插入 时钟调用语句
     * 如果if语句的子语句 中 不是块语句的 不需要 插入 时钟调用语句, 因此也就没有 形如 processThenStmt、processElseStmt 之类的自定义处理了。
     *
     * 举例如下:
     * if(...)
     *    ...; //这里是if的then子语句， 该then子语句 不是 块语句，不需要插入 时钟调用语句.
     * else {//这里是if的else子语句， 该else子语句 是 块语句， 会经过 转交: TraverseStmt(child) ---...---> TraverseCompoundStmt(child) , 最终在 TraverseCompoundStmt中 对 该else子语句中的每条语句前插入 时钟调用语句.
     *  ...;
     * }
     * @param ifStmt
     * @return
     */
    virtual bool TraverseIfStmt(IfStmt *ifStmt);
    /**
     * 1. while语句整体 前 插入 时钟调用语句
     * 2. 粘接直接子节点到递归链
     * 不直接处理while语句内的子语句
     *
     * 当while语句的子语句 child:[body即循环体]  循环体是块语句的情况下, 循环体内才需要 插入 时钟调用语句, 这是经转交 TraverseStmt(body) ---...---> TraverseCompoundStmt(body) 后，由TraverseCompoundStmt(body)对该块语句body中的每条语句前 插入 时钟调用语句.
     * 如果while语句的循环体 是一个单行语句 即不是块语句，  则 不需要 在该单行语句前 插入 时钟调用语句。
     *
     * 举例如下:
     * while(...)
     *    ...; //这里是while的循环体， 该循环体 不是 块语句，故而 该 循环体前 不需要插入 时钟调用语句.
     *
     * while(...)
     * {
     *    ...; //这里是while的循环体， 该循环体 是 块语句，会经过 转交: TraverseStmt(循环体) ---...---> TraverseCompoundStmt(循环体) , 最终在 TraverseCompoundStmt中 对 该循环体中的每条语句前插入 时钟调用语句.
     * }
     * @param whileStmt
     * @return
     */
    virtual bool TraverseWhileStmt(WhileStmt *whileStmt);
    /* for、while很相似 */
    /**
     * 1. for语句整体 前 插入 时钟调用语句
     * 2. 粘接直接子节点到递归链
     * 不直接处理for语句内的子语句
     *
     * 当for语句的子语句 child:[body即循环体]  循环体是块语句的情况下, 循环体内才需要 插入 时钟调用语句, 这是经转交 TraverseStmt(body) ---...---> TraverseCompoundStmt(body) 后，由TraverseCompoundStmt(body)对该块语句body中的每条语句前 插入 时钟调用语句.
     * 如果for语句的循环体 是一个单行语句 即不是块语句，  则 不需要 在该单行语句前 插入 时钟调用语句。
     *
     * 举例如下:
     * for(...)
     *    ...; //这里是for的循环体， 该循环体 不是 块语句，故而 该 循环体前 不需要插入 时钟调用语句.
     *
     * for(...)
     * {
     *    ...; //这里是for的循环体， 该循环体 是 块语句，会经过 转交: TraverseStmt(循环体) ---...---> TraverseCompoundStmt(循环体) , 最终在 TraverseCompoundStmt中 对 该循环体中的每条语句前插入 时钟调用语句.
     * }
     * @param forStmt
     * @return
     */
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
    const std::shared_ptr<Rewriter> mRewriter_ptr;

public:
    ASTContext *Ctx;
    CompilerInstance& CI;
    SourceManager& SM;

    /** 存放有constexpr修饰的函数整体坐标范围
     * 方便 processStmt 判断 当前语句 是否在这些函数坐标范围内，若是 则不插入语句
     */
    std::list<SourceRange> constexpr_func_ls;

    /**其前已经插入语句的 节点ID 们, 为防止重复遍历导致的重复插入，
     * 节点ID集合（防重复插入） 应该按 分配、释放 分开，从而互不干扰
     * 可达到： 即使重复遍历了 但不会重复插入
     * 如果后面发现ID 不是全局唯一的 可以尝试换成 该节点的开始位置
     */
    std::set<int64_t> allocInsertedNodeIDLs;
    std::set<int64_t> freeInsertedNodeIDLs;



};


#endif