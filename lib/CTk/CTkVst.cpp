#include "CTk/CTkVst.h"

#include "clang/AST/AST.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "CTk/Util.h"

using namespace clang;

#include <iostream>
#include <clang/AST/ParentMapContext.h>

using namespace llvm;
using namespace clang;

//-----------------------------------------------------------------------------
// CTkVst implementation
//-----------------------------------------------------------------------------
/*
运行clang++带上本插件.so：但这只是cc1  如何能把整个编译过程串起来？
/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/bin/clang++ -cc1  -load /pubx/clang-tutor/cmake-build-debug/lib/libCTk.so   -plugin CTk   test_main.cpp


只运行了本插件CTk，没有运行编译过程:
#参考: https://releases.llvm.org/8.0.0/tools/clang/docs/ClangPlugins.html
/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/bin/clang++  -Xclang   -load -Xclang /pubx/clang-tutor/cmake-build-debug/lib/libCTk.so  -Xclang   -plugin -Xclang  CTk  -c  /pubx/clang-tutor/test/test_main.cpp

"-plugin" 改为  "-add-plugin", 运行了编译过程:  并输出了 test_main.o
#参考: https://www.ibm.com/docs/en/xl-c-and-cpp-linux/16.1.0?topic=cla-running-user-defined-actions-by-using-clang-plug-ins
/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/bin/clang++  -Xclang   -load -Xclang /pubx/clang-tutor/cmake-build-debug/lib/libCTk.so  -Xclang   -add-plugin -Xclang  CTk  -c  /pubx/clang-tutor/test/test_main.cpp


运行clang++带上本插件.so 且 运行编译、链接 全过程:
/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/bin/clang++  -Xclang   -load -Xclang /pubx/clang-tutor/cmake-build-debug/lib/libCTk.so  -Xclang   -add-plugin -Xclang  CTk   /pubx/clang-tutor/t_clock_tick/test_main.cpp  -o test_main
但运行应用，应用结束时 t没变依然是0，说明本插件对源码的修改没生效.



 */


//std::set<FileID> CTkVst::fileInsertedIncludeStmt;//={};//删除fileInsertedIncludeStmt，不再对间接文件做插入，目前只插入直接文件。
const std::string CTkVst::funcName_TCTk = "X__t_clock_tick";
const std::string CTkVst::IncludeStmt_TCTk = "#include \"t_clock_tick.h\"\n";



static auto _whileStmtAstNodeKind=ASTNodeKind::getFromNodeKind<WhileStmt>();
static auto _forStmtAstNodeKind=ASTNodeKind::getFromNodeKind<ForStmt>();
static auto _returnStmtAstNodeKind=ASTNodeKind::getFromNodeKind<ReturnStmt>();
static auto _unaryOperatorAstNodeKind=ASTNodeKind::getFromNodeKind<UnaryOperator>();
static auto _implicitCaseExprAstNodeKind=ASTNodeKind::getFromNodeKind<ImplicitCastExpr>();
static auto _CXXStaticCastExprAstNodeKind=ASTNodeKind::getFromNodeKind<CXXStaticCastExpr>();
static auto _ExprAstNodeKind=ASTNodeKind::getFromNodeKind<Expr>();
static auto _CompoundStmtAstNodeKind=ASTNodeKind::getFromNodeKind<CompoundStmt>();

bool shouldInsert(Stmt *S,ASTNodeKind& parent0NodeKind){
//Stmt::StmtClass & stmtClass
  Stmt::StmtClass stmtClass = S->getStmtClass();

  //父亲0节点 若不是组合语句，则不插入
  //   只在组合语句内插入
  if(!parent0NodeKind.isSame(_CompoundStmtAstNodeKind)){
    return false;
  }

  //父亲0节点是表达式的子类吗？
  bool parent0IsAKindExpr=_ExprAstNodeKind.isBaseOf(parent0NodeKind);


  ////{这一组条件，估计不起作用了，都被 条件 "只在组合语句内插入" 屏蔽了。所以这些条件可以暂时去掉了
/*  //{内部 不可扩展 的 语法节点 内 是不能插入更多语法结构的 否则语法错误
  //无大括号循环内语句前不要插入， 若要插入，需要先加大括号。
  if(parent0NodeKind.isSame(_forStmtAstNodeKind) || parent0NodeKind.isSame(_whileStmtAstNodeKind)
  //return语句内的语句前不要插入，否则语法错误。
  ||parent0NodeKind.isSame(_returnStmtAstNodeKind)
  //非操作符内的语句前不要插入，否则语法错误。
  ||parent0NodeKind.isSame(_unaryOperatorAstNodeKind)
  //隐式类型转换内的语句前不要插入，否则语法错误。
  ||parent0NodeKind.isSame(_implicitCaseExprAstNodeKind)
  //static_cast静态类型转换内的语句前不要插入，否则语法错误。
  ||parent0NodeKind.isSame(_CXXStaticCastExprAstNodeKind)
  //若父亲0节点是表达式的子类吗，则肯定其内肯定不能插入语句的。
  //    即 所有的表达式内都不能插入 语句，否则语法报错。
  || parent0IsAKindExpr
  ){
    //如果当前语句S的父亲节点是for语句头，则不插入时钟语句. 单行for语句包含 语句S， 语句S前肯定不能插入，否则 语义不对 甚至 可能语法错误 比如 变量没声明。
    return false;
  }*/
  ////}

  switch (stmtClass) {//switch开始
    //{不插入时钟语句概率大的情况
    case Stmt::CompoundStmtClass:{
      return false;
    }
    case Stmt::ImplicitCastExprClass:{ //隐式类型转换
      return false;
    }
    case Stmt::StringLiteralClass:{
      return false;
    }
    case Stmt::IntegerLiteralClass:{
      return false;
    }
    case Stmt::DeclRefExprClass:{//存疑,待找到实例以验证
      return false;
    }
    //}

    //{插入时钟语句概率大的情况
    case Stmt::DeclStmtClass:{
      if(parent0NodeKind.isSame(_forStmtAstNodeKind) ){
        //如果当前语句S的父亲节点是for语句头，则不插入时钟语句.
        return false;
      }
      return true;
    }
    case Stmt::CallExprClass:{
      {
      ASTNodeKind compoundStmtAstNodeKind=ASTNodeKind::getFromNodeKind<CompoundStmt>();
      if(parent0NodeKind.isSame(compoundStmtAstNodeKind)){
        //如果调用语句CallExpr的父亲节点是组合语句CompoundStmt，则插入时钟语句. 举例如下:
        // 组合语句CompoundStmt: "{ int age; func1();}" , 其中 "func1();" 是调用语句CallExpr.
        return true;
      }
      }

      {
      ASTNodeKind varDeclAstNodeKind=ASTNodeKind::getFromNodeKind<VarDecl>();
      if(parent0NodeKind.isSame(varDeclAstNodeKind)){
        //如果调用语句CallExpr的父亲节点是变量声明语句VarDecl，则不插入时钟语句. 举例如下:
        // 变量声明语句VarDecl: "float cost=calcCost(3,false);" , 其中 "calcCost(3,false)" 是调用语句CallExpr.
        return false;
      }
      }

      {
      ASTNodeKind binaryOperatorAstNodeKind=ASTNodeKind::getFromNodeKind<BinaryOperator>();
      if(parent0NodeKind.isSame(binaryOperatorAstNodeKind)){
        //如果调用语句CallExpr的父亲节点是 二元操作符BinaryOperator，则不插入时钟语句. 举例如下:
        // 二元操作符BinaryOperator 语句: "name=getName(true);" , 其中 "getName(true)" 是调用语句CallExpr, 二元操作符BinaryOperator 这里是指 赋值号"="
        return false;
      }
      }

      {
      ASTNodeKind ifStmtAstNodeKind=ASTNodeKind::getFromNodeKind<IfStmt>();
      if(parent0NodeKind.isSame(ifStmtAstNodeKind)){
        //如果调用语句CallExpr的父亲节点是 判断语句IfStmt，则不插入时钟语句，否则破坏了原来意思. 举例如下:
        // 判断语句IfStmt  : "if(done) do_job();" , 其中 "do_job()" 是调用语句CallExpr
        return false;
      }
      }

      return true;
    }
    case Stmt::ForStmtClass: {//for循环整体
      return true;
    }
//    case Stmt::UnaryOperatorClass://一元操作符语句,  这里 暂时不插入. 因为需要知道当前是在for循环第3位置 , 还是单独一条语句. for循环第3位置前插入 分割符只能是逗号, 单独一条语句前插入 分割符只能是分号.
//    case Stmt::BinaryOperatorClass://二元操作符语句，暂时不插入。 需要看其内部组成和外部包裹，比如若是被if()包裹 肯定其前肯定不能插入，如果是单独一条语句 其前可以插入。
    case Stmt::IfStmtClass: {//if语句整体
      return true;
    }
    case Stmt::ReturnStmtClass:{
      return true;
    }
    //}
  }//switch结束


  //默认不插入
  return false;
}









/**给定源文件路径是否系统源文件
 * 系统源文件路径举例：
/usr/lib/gcc/x86_64-linux-gnu/11/../../../../include/c++/11/bits/cpp_type_traits.h
/usr/lib/gcc/x86_64-linux-gnu/11/../../../../include/c++/11/ext/type_traits.h
/usr/include/x86_64-linux-gnu/bits/iscanonical.h

/app/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/lib/clang/15.0.0/include/uintrintrin.h
 * @param fn
 * @return
 */
bool isInternalSysSourceFile(StringRef fn) {
  bool startWithUsr=fn.startswith("/usr/");
  bool isLLVM01=fn.startswith("/app/llvm_release_home/clang+llvm");
  bool isLLVM02=fn.startswith("/llvm_release_home/clang+llvm");
  bool isInternal=(startWithUsr||isLLVM01||isLLVM02);
  return isInternal;
}
void insertBefore_X__t_clock_tick(Rewriter &rewriter, SourceLocation sourceLocation, int stackVarAllocCnt, int stackVarFreeCnt, int heapObjAllocCnt, int heapObjcFreeCnt){
  char cStr_X__t_clock_tick[256];

  //X__t_clock_tick(int stackVarAllocCnt, int stackVarFreeCnt, int heapObjAllocCnt, int heapObjcFreeCnt)
  sprintf(cStr_X__t_clock_tick, "%s(%d, %d, %d, %d);\n", CTkVst::funcName_TCTk.c_str(),stackVarAllocCnt,stackVarFreeCnt,heapObjAllocCnt,heapObjcFreeCnt);//"X__t_clock_tick(%d, %d, %d, %d)"
  llvm::StringRef strRef_X__t_clock_tick(cStr_X__t_clock_tick);

//  mRewriter.InsertTextAfter(S->getEndLoc(),"/**/");
  rewriter.InsertTextBefore(sourceLocation,strRef_X__t_clock_tick);

}

/**遍历语句
 *
 * @param stmt
 * @return
 */
bool CTkVst::processStmt(Stmt *stmt){
  int64_t stmtId = stmt->getID(*Ctx);

  SourceManager & SM = mRewriter.getSourceMgr();
  const LangOptions & langOpts = mRewriter.getLangOpts();


  SourceLocation beginLoc=stmt->getBeginLoc();
  SourceRange sourceRange=stmt->getSourceRange();
  FileID fileId = SM.getFileID(beginLoc);

  FileID mainFileId = SM.getMainFileID();

  std::string stmtFileAndRange=sourceRange.printToString(SM);

  //获取当前语句S的源码文本
  std::string stmtSourceText=Util::getSourceTextBySourceRange(stmt->getSourceRange(), SM, langOpts);

  if(mainFileId!=fileId){
//    Util::printStmt(CI,"查看","暂时不对间接文件插入时钟语句",stmt, true); //开发用打印
    return true;
  }

  Stmt::StmtClass stmtClass = stmt->getStmtClass();
  const char* stmtClassName = stmt->getStmtClassName();


//  std::cout << "[#" << stmtSourceText << "#]:{#" << stmtClassName << "#}" ;  //开发用打印

  //{开发用，条件断点
//  bool shouldBreakPointer=stmtSourceText=="f111(";
//  bool shouldBreakPointer2=stmtSourceText=="!f111(";
  bool BUG04= (stmtSourceText=="malloc(AllocSize");//BUG04出现条件
  //}

  DynTypedNodeList parentS=this->Ctx->getParents(*stmt);
  size_t parentSSize=parentS.size();
  if(parentSSize>1){
    char msg[128];
    sprintf(msg,"注意:父节点个数大于1, 为:%d",parentSSize);
    Util::printStmt(*Ctx, CI, "查看", msg, stmt, true);
  }
  if(parentSSize<=0){
    return true;
  }
  auto parent0 = parentS[0];
  ASTNodeKind parent0NodeKind=parentS[0].getNodeKind();
  const char * parent0NodeKindCStr=parent0NodeKind.asStringRef().str().c_str();


  StringRef fn;
  Util::getSourceFilePathOfStmt(stmt, SM, fn);
  std::string fnStr=fn.str();

  bool _isInternalSysSourceFile  = isInternalSysSourceFile(fn);
  bool _shouldInsert=shouldInsert(stmt, parent0NodeKind);

//  char msg[256];
//  sprintf(msg,"parent0NodeKind:%s,_isInternalSysSourceFile:%d,_shouldInsert:%d",parent0NodeKindCStr,_isInternalSysSourceFile,_shouldInsert);//sprintf中不要给 clang::StringRef类型，否则结果是怪异的。
//  Util::printStmt(*Ctx, CI, "查看_VisitStmt", msg, stmt, true);  //开发用打印

  if( ( !_isInternalSysSourceFile ) && _shouldInsert){

    int stackVarAllocCnt=0;
    int stackVarFreeCnt=0;
    int heapObjAllocCnt=0;
    int heapObjcFreeCnt=0;
    insertBefore_X__t_clock_tick(mRewriter, stmt->getBeginLoc(), stackVarAllocCnt, stackVarFreeCnt, heapObjAllocCnt,
                                 heapObjcFreeCnt);

    char msgz[256];
    sprintf(msgz,"%p,插入时钟语句",&mRewriter);
    //这里打印说明: mRewriter 地址 有两种值。有某个地方再次造了新的Rewriter，导致后一个结果覆盖了前一个结果，前一个结果丢失。应该一直用同一个mRewriter
    Util::printStmt(*Ctx, CI, "插入调用", msgz, stmt, false);  //开发用打印

  }else{
//  Util::printStmt(CI,"不插入","not insert X__t_clock_tick",stmt, false);  //开发用打印
  }
  return true;
}


/*bool CTkVst::VisitCallExpr(CallExpr *callExpr){

}*/

bool CTkVst::TraverseCompoundStmt(CompoundStmt *compoundStmt  ){
  const Stmt::child_range &subStmtLs = compoundStmt->children();

  ///////////////计算 子语句列表 中 变量声明语句个数，以生成释放语句 并插入
  //此组合语句内的变量声明语句个数
  int declStmtCnt=0;

  for(Stmt* stmt:subStmtLs){
    const char *stmtClassName = stmt->getStmtClassName();
    Stmt::StmtClass stmtClass = stmt->getStmtClass();
    if(Stmt::DeclStmtClass==stmtClass){
      declStmtCnt++;
    }
    Util::printStmt(*Ctx,CI,"查看组合语句内子语句类型","",stmt,true);
  }
  //时钟语句默认插入位置是 组合语句 右花括号} 前
  SourceLocation insertLoc=compoundStmt->getRBracLoc();

  Stmt *endStmt = compoundStmt->body_back();
  if(endStmt){
    Stmt::StmtClass endStmtClass = endStmt->getStmtClass();
    //若组合语句内最后一条语句是 return语句，则 时钟语句默认插入位置 改为 该return语句前.
    if(Stmt::ReturnStmtClass==endStmtClass){
      insertLoc=endStmt->getBeginLoc();
    }
  }

  //本块内有声明变量，才会插入释放语句
  if(declStmtCnt>0){
  int stackVarAllocCnt=0;
  int stackVarFreeCnt=declStmtCnt;
  int heapObjAllocCnt=0;
  int heapObjcFreeCnt=0;
  insertBefore_X__t_clock_tick(mRewriter, insertLoc, stackVarAllocCnt, stackVarFreeCnt, heapObjAllocCnt, heapObjcFreeCnt);
  }

  ///////////////处理  子语句列表 中每条语句

  for(Stmt* stmt:subStmtLs){
    processStmt(stmt);
    TraverseStmt  (stmt);
  }

  TraverseStmt  (compoundStmt);//这句造成死递归. 显然是 无条件环 造成的， 本行语句 的 函数+参数  一定是 完全相同地  出现  在当前调用栈 的上层，且 没有条件来斩断此环， 才会向上调用形成死递归。
  // 由此说明 clang内部假设了 ： 在Stmt的递归过程中  TraverseStmt 是上层  而各个 TraverseXxxStmt 是下层
  /**大量重复:
#42449 0x0000555555c320ea in CTkVst::TraverseCompoundStmt (this=0x555557ddd238, compoundStmt=0x555557e445d8) at /pubx/clang_plugin_demo/clang-tutor/lib/CTk/CTkVst.cpp:349
#42450 0x0000555555b830e0 in clang::RecursiveASTVisitor<CTkVst>::dataTraverseNode (this=0x555557ddd238, S=0x555557e445d8, Queue=0x7fffffffc950) at /llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/include/clang/AST/StmtNodes.inc:73
#42451 0x0000555555b2be37 in clang::RecursiveASTVisitor<CTkVst>::TraverseStmt (this=0x555557ddd238, S=0x555557e445d8, Queue=0x0) at /llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/include/clang/AST/RecursiveASTVisitor.h:700
#42452 0x0000555555c320ea in CTkVst::TraverseCompoundStmt (this=0x555557ddd238, compoundStmt=0x555557e445d8) at /pubx/clang_plugin_demo/clang-tutor/lib/CTk/CTkVst.cpp:349

   调用栈深度高达 42476，最终 调用栈溢出 异常退出

   */

/**  推测的结论：
在clang源码内部 ：
   在Stmt递归过程中， TraverseStmt 位于 顶层， 各个 TraverseXxxStmt 位于 下层， 即   TraverseStmt 调用 各个 TraverseXxxStmt
在clang源码内部  Stmt递归过程 伪码如下：
TraverseStmt(第k层节点x){
	for (Xxx in [If, While, For, Try, ... ] ){
		TraverseXxxStmt(x);
	}
//因此 TraverseXxxStmt(x) 中 不能含有 TraverseStmt(x) , 否则 形成  环 且 是无条件环  即死递归
//  clang并没有预期允许能发生这种事情 即没有代码能检测这种事情   即 是 无条件
}

// 也可以看出 自定义 TraverseXxxStmt(x) 应该具有如下形式：
//写法1. 正确的 自定义 TraverseXxxStmt(x) 写法 ：
TraverseXxxStmt(x) {
//  对当前节点x做想要的自定义处理， 比如 插入 时钟调用语句

//  将递归链条正确的接好:
  for(auto child: x){
    TraverseStmt(child); //进入下一层节点的递归. 注意 此时 调用栈上层有 TraverseStmt(x),  本行有 TraverseStmt(child) ， 但 x 和 child 是 不相等的 ，所以不存在环 即不会死递归。
    // 注：  x 是 语法树 中的 第k层节点  ，而 child是第k+1层节点，因此 x 和 child 不相等。
  }
}

//写法2. 错误的自定义写法： 引起死递归
TraverseXxxStmt(x){
    //对当前节点x做想要的自定义处理

    TraverseStmt(x);// 这里想图省事，直接用本层节点. 注意 此时 调用栈上层有 TraverseStmt(x),  本行又有  TraverseStmt(x) ，二者 函数+参数 完全相同  ，所以存在环 且 没有条件斩断环 即 产生死递归。
}
 */
//  Util::printStmt(*Ctx,CI,"查看","组合语句",compoundStmt,false);
}


bool CTkVst::TraverseIfStmt(IfStmt *ifStmt){
  std::all_of(ifStmt->children().begin(), ifStmt->children().end(),
[this](Stmt* childK){
      Util::printStmt(*this->Ctx,this->CI,"查看VisitIfStmt","的孩子",childK, true);
      return true;
    }
  );

  //////

//  for (auto child:ifStmt.getBody()){//思路伪代码
//    processStmt(child);
//  }
  return true;
}
bool CTkVst::VisitWhileStmt(WhileStmt *whileStmt){
/*  for (auto child:whileStmt.getBody()){//思路伪代码
    processStmt(child);
  }*/
  return true;
}

/*bool CTkVst::VisitCXXMethodDecl(CXXMethodDecl *declK) {

//  FunctionDecl *functionDecl = declK->getAsFunction();
//  printf("functionDecl:%d,\n",functionDecl);
//  if(functionDecl){
//          bool _isConstexpr = functionDecl->isConstexpr();
    ConstexprSpecKind constexprKind = declK->getConstexprKind();
    printf("constexprKind:%d,\n",constexprKind);
    if(ConstexprSpecKind::Constexpr==constexprKind){
      //跳过constexpr修饰的函数
      //  constexpr修饰的函数 不能插入non-constexpr函数调用, 否则  c++编译错误。似语义错误,非语法错误。
      Util::printDecl(*Ctx, CI, "查看", "发现Constexpr修饰的函数", declK, false);
//      break;//此时应该给一个标记，告知下层VisitStmt：你语句处在不可插入 时钟调用语句 的Constexpr函数中。
//      但 做不到 上告诉下 ， 唯一的办法是 下往上找直到找到函数节点为止 才能发现本函数被修饰, 从而不做插入。
    }
//  }
}*/
/*bool CTkVst::VisitFunctionDecl(FunctionDecl *Decl) {
  return true;
}*/

/*bool CTkVst::VisitVarDecl(VarDecl *Decl) {
  return true;
}*/

/*bool CTkVst::VisitFieldDecl(FieldDecl *Decl) {

  return true;
}*/
