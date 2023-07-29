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

//TODO 处理 要忽略 t_clock_tick.cpp 和 t_clock_tick.h

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

    void insertAfter_X__funcEnter(int64_t funcDeclId, SourceLocation funcBodyLBraceLoc , const char* whoInserted);
    void insertBefore_X__funcReturn( int64_t returnStmtId, SourceLocation stmtBeginLoc , const char* whoInserted);
    void insertAfter_X__funcReturn( int64_t funcBodyEndStmtId, SourceLocation funEndStmtEndLoc , const char* whoInserted);
    void insert_X__funcReturn(bool before, int64_t flagStmtId, SourceLocation insertLoc , const char* whoInserted);
    void insertBefore_X__t_clock_tick(LifeStep lifeStep, int64_t stmtId, SourceLocation stmtBeginLoc, int stackVarAllocCnt, int stackVarFreeCnt, int heapObjAllocCnt, int heapObjcFreeCnt, const char* whoInserted=NULL);

    /**void函数、构造函数 最后一条语句若不是return，则需在最后一条语句之后插入  函数释放语句
     * @param functionDecl
     * @param whoInserted
     */
    void insert_X__funcReturn_whenVoidFuncOrConstructorNoEndReturn(FunctionDecl *functionDecl , const char* whoInserted);

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
    /* for、while、doWhile很相似 */
    /**
     * 1. while语句整体 前 插入 时钟调用语句
     * 2. 粘接直接子节点到递归链
     * 不直接处理while语句内的子语句
     *
     * 当while语句的子语句 child:[body即循环体]  循环体是块语句的情况下, 循环体内才需要 插入 时钟调用语句, 这是经转交 TraverseStmt(循环体) ---...---> TraverseCompoundStmt(循环体) 后，由TraverseCompoundStmt(循环体)对该循环体中的每条语句前 插入 时钟调用语句.
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
    /* for、while、doWhile很相似 */
    /**
     * 1. for语句整体 前 插入 时钟调用语句
     * 2. 粘接直接子节点到递归链
     * 不直接处理for语句内的子语句
     *
     * 当for语句的子语句 child:[body即循环体]  循环体是块语句的情况下, 循环体内才需要 插入 时钟调用语句, 这是经转交 TraverseStmt(body) ---...---> TraverseCompoundStmt(循环体) 后，由TraverseCompoundStmt(循环体)对该循环体中的每条语句前 插入 时钟调用语句.
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
    /**
     * 1. C++Try语句整体 前 插入 时钟调用语句
     * 2. 粘接直接子节点到递归链
     * 不直接处理C++Try语句内的尝试体
     *
     * C++Try语句的尝试体 一定是 块语句,   对 尝试体 内  插入 时钟调用语句, 这是经转交 TraverseStmt(尝试体) ---...---> TraverseCompoundStmt(尝试体) 后，由TraverseCompoundStmt(尝试体)对该尝试体中的每条语句前 插入 时钟调用语句.
     *
     * 举例如下:
     *
     * try
     * {
     *    ...; //这里是C++Try的尝试体， 该尝试体  会经过 转交: TraverseStmt(尝试体) ---...---> TraverseCompoundStmt(尝试体) , 最终在 TraverseCompoundStmt中 对 该尝试体中的每条语句前插入 时钟调用语句.
     * }
     * @param forStmt
     * @return
     */
    virtual bool TraverseCXXTryStmt(CXXTryStmt *cxxTryStmt);
    /**
     * 1. C++Catch语句整体 前 插入 时钟调用语句
     * 2. 粘接直接子节点到递归链
     * 不直接处理C++Catch语句内的捕捉体
     *
     * C++Catch语句的捕捉体 一定是 块语句,   对 捕捉体 内  插入 时钟调用语句, 这是经转交 TraverseStmt(捕捉体) ---...---> TraverseCompoundStmt(捕捉体) 后，由TraverseCompoundStmt(捕捉体)对该捕捉体中的每条语句前 插入 时钟调用语句.
     *
     * 举例如下:
     * try{...}
     * catch(...)
     * {
     *    ...; //这里是C++Catch的捕捉体， 该捕捉体  会经过 转交: TraverseStmt(捕捉体) ---...---> TraverseCompoundStmt(捕捉体) , 最终在 TraverseCompoundStmt中 对 该捕捉体中的每条语句前插入 时钟调用语句.
     * }
     * @param forStmt
     * @return
     */
    virtual bool TraverseCXXCatchStmt(CXXCatchStmt *cxxCatchStmt);
    /* for、while、doWhile很相似 */
    /**
     * 1. doWhile语句整体 前 插入 时钟调用语句
     * 2. 粘接直接子节点到递归链
     * 不直接处理doWhile语句内的子语句
     *
     * 当doWhile语句的子语句 child:[body即循环体]  循环体是块语句的情况下, 循环体内才需要 插入 时钟调用语句, 这是经转交 TraverseStmt(循环体) ---...---> TraverseCompoundStmt(循环体) 后，由TraverseCompoundStmt(循环体)对该循环体中的每条语句前 插入 时钟调用语句.
     * 如果doWhile语句的循环体 是一个单行语句 即不是块语句，  则 不需要 在该单行语句前 插入 时钟调用语句。
     *
     * 举例如下:
     * do
     * ...;    //这里是doWhile的循环体， 该循环体 不是 块语句，故而 该 循环体前 不需要插入 时钟调用语句.
     * while(...);
     *
     * do
     * {
     *    ...; //这里是doWhile的循环体， 该循环体 是 块语句，会经过 转交: TraverseStmt(循环体) ---...---> TraverseCompoundStmt(循环体) , 最终在 TraverseCompoundStmt中 对 该循环体中的每条语句前插入 时钟调用语句.
     * }
     * while(...);
     * @param doStmt
     * @return
     */
    virtual bool TraverseDoStmt(DoStmt *doStmt);
    /**
     * 1. switch语句整体 前 插入 时钟调用语句
     * 2. 粘接直接子节点到递归链
     * 不直接处理switch语句内的子语句
     *
     *
     * switch语句的多情况体 一定是块语句 ，多情况体内的子语句是case语句 ，case语句整体前是不能插入 不需要插入 时钟调用语句.
     *
     * 举例如下:
     * switch(...)
     * {       //这里是switch的多情况体
     *
     * case 1: //这里是多情况体 的 第1个 子语句 case语句1
     * {
     * }//case语句1结束
     *
     * case 2: //这里是多情况体 的 第2个 子语句 case语句2
     * break;
     * //case语句2结束
     *
     * case 3: //这里是多情况体 的 第3个 子语句 case语句3
     * {
     * ...;
     * }//case语句3结束
     *
     * }//多情况体结束
     *
     * @param switchStmt
     * @return
     */
    virtual bool TraverseSwitchStmt(SwitchStmt *switchStmt);
    /**
     * 1. case语句整体 前 不插入 时钟调用语句
     * 2. 粘接直接子节点到递归链
     * 不直接处理case语句内的子语句
     *
     * case语句的子语句 child:[body即单情况体]  单情况体内的子语句是case语句 ，case语句整体前是不能插入 不需要插入 时钟调用语句.
     *
     * 当case语句的子语句 child:[body即单情况体]  单情况体是块语句的情况下, 单情况体内才需要 插入 时钟调用语句, 这是经转交 TraverseStmt(单情况体) ---...---> TraverseCompoundStmt(单情况体) 后，由TraverseCompoundStmt(单情况体)对该单情况体中的每条语句前 插入 时钟调用语句.
     * 如果case语句的单情况体 是一个单行语句 即不是块语句，  则 不需要 在该单行语句前 插入 时钟调用语句。
     *
     *
     * 举例如下:
     * switch(...)
     * {//这里是switch语句的多情况体
     *
     * case 1: //这里是 case语句1
     * {  //这里是 case语句1 的  单情况体
     * ...;
     * }//case语句1结束
     *
     * case 2: //这里是 case语句2
     * break; ////这里是 case语句2 的  单情况体
     * //case语句2结束
     *
     * case 3: //这里是 case语句3
     * {  //这里是 case语句3 的  单情况体
     * ...;
     * }//case语句3结束
     *
     * }//多情况体结束
     *
     * @param switchStmt
     * @return
     */
    virtual bool TraverseCaseStmt(CaseStmt *caseStmt);//由于case语句前不能插入 任何语句 ,否则语法错误，因此 case语句不需要自定义处理，只需要对case语句用clang内部的正常递归即可。
    //这里应该有 所有能带块的语句: if块、while块、else块、for块、switch块、try块、catch块...

    /////////constexpr
    virtual bool TraverseFunctionDecl(FunctionDecl* functionDecl);
    virtual bool TraverseCXXMethodDecl(CXXMethodDecl* cxxMethodDecl);
    virtual bool TraverseCXXConstructorDecl(CXXConstructorDecl* cxxConstructorDecl);
    //TODO: c++析构函数遍历
//    virtual bool TraverseCXXDestructorDecl(CXXDestructorDecl * cxxDestructorDecl);


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
  bool hasBody,
  int64_t funcDeclID,
  Stmt *funcBodyStmt,
  const char *whoInsertedFuncReturn);


    /** 函数入口  前 插入 检查语句: 检查 上一个返回的 是否 释放栈中其已分配变量 ，如果没 则要打印出错误消息，以方便排查问题。
     *
     * @param funcBody
     * @param functionDeclID
     * @param whoInserted
     */
    void __wrap_insertAfter_X__funcEnter(Stmt *funcBody,int64_t functionDeclID , const char* whoInserted);

    /**
     return语句  前 插入 释放栈变量语句： 以释放此时本函数已经分配的全部栈变量，有两个方案：
     1. 运行时计数：这是简单方案，如下:
     在运行时 算出  本函数当前已经申请的所有栈变量,
          可以在栈变量分配时计数，这里目前使用此方案

     2. 编译时解析各级父块栈变量分配： 这是复杂方案，如下：
     编译源代码时解析语法结构 算出 本函数当前已经申请的所有栈变量 ，即 ：
               当前块以及当前块的各级父亲块内已经申请的栈变量。
     */
    virtual bool TraverseReturnStmt(ReturnStmt *returnStmt);


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


    /**其前已经插入语句的 节点ID 们, 为防止重复遍历导致的重复插入，
     * 节点ID集合（防重复插入） 应该按 分配、释放 分开，从而互不干扰
     * 可达到： 即使重复遍历了 但不会重复插入
     * 如果后面发现ID 不是全局唯一的 可以尝试换成 该节点的开始位置
     */
    std::set<int64_t> allocInsertedNodeIDLs;
    std::set<int64_t> freeInsertedNodeIDLs;


    std::set<int64_t> funcReturnInsertedNodeIDLs;
    std::set<int64_t> funcEnterInsertedNodeIDLs;



};


#endif