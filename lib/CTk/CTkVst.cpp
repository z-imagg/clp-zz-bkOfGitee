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


static auto _VarDeclAstNodeKind=ASTNodeKind::getFromNodeKind<VarDecl>();

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

void insertBefore_X__t_clock_tick(Rewriter &rewriter, SourceLocation sourceLocation, int stackVarAllocCnt, int stackVarFreeCnt, int heapObjAllocCnt, int heapObjcFreeCnt,const char* whoInserted=NULL){
  char cStr_X__t_clock_tick[256];

  char _comment[90]="";
  if(whoInserted){
    //如果有提供，插入者信息，则放在注释中.
    sprintf(_comment,"//%s",whoInserted);
  }

  //X__t_clock_tick(int stackVarAllocCnt, int stackVarFreeCnt, int heapObjAllocCnt, int heapObjcFreeCnt)
  sprintf(cStr_X__t_clock_tick, "%s(/*栈生*/%d, /*栈死*/%d, /*堆生*/%d, /*堆死*/%d);%s\n", CTkVst::funcName_TCTk.c_str(),stackVarAllocCnt,stackVarFreeCnt,heapObjAllocCnt,heapObjcFreeCnt,_comment);//"X__t_clock_tick(%d, %d, %d, %d)"
  llvm::StringRef strRef_X__t_clock_tick(cStr_X__t_clock_tick);

//  mRewriter.InsertTextAfter(S->getEndLoc(),"/**/");
  rewriter.InsertTextBefore(sourceLocation,strRef_X__t_clock_tick);

}

//TODO 暂时去掉不必要的打印
//TODO 分配变量个数： 当前语句如果是VarDecl

/**遍历语句
 *
 * @param stmt
 * @return
 */
//TODO 喂给processStmt 就是其前肯定能插入的  ， processStmt 不需要再判断是否能插入的问题了？
//TODO 插入前 需要看该语句ID是否已经被插入（ 还是 看 该位置 是否已经被插入？ ）  这两者没区别。 关键是  理论上 rewrite.overwriteChangedFiles 是在 HandleTranslationUnit 结尾 才发生，    所以 这种判断才没有被破坏  才能用。
//    比如 对if语句前 TraverseCompoundStmt 和 TraverseIfStmt 都会插入 ， 这就重复了
bool CTkVst::processStmt(Stmt *stmt,const char* whoInserted){
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

//    stmtClass=stmt->getStmtClass();
    int stackVarAllocCnt=0;
    int stackVarFreeCnt=0;
    int heapObjAllocCnt=0;
    int heapObjcFreeCnt=0;
    if(stmtClass==Stmt::StmtClass::DeclStmtClass){
      //如果当前语句是声明语句
      DeclStmt *declStmt = static_cast<DeclStmt *>(stmt);
      if(declStmt){

        Decl *decl0 = *(declStmt->decl_begin());
        if(decl0 && decl0->getKind()==Decl::Kind::Var){
          //如果当前语句是声明语句, 且第一个子声明是变量声明语句,则栈变量分配个数填写1
          //  有可能是这种样子: int n,m,u,v=0;  应该取 declStmt->decls() 的size
          const DeclStmt::decl_range &declRange = declStmt->decls();
          // 取 declStmt->decls() 的size
          long declCnt = std::distance(declRange.begin(), declRange.end());
          stackVarAllocCnt=declCnt;
        }
      }
      //stmtClass==Stmt::StmtClass::DeclStmtClass
//      stmt->
    }
    insertBefore_X__t_clock_tick(mRewriter, stmt->getBeginLoc(), stackVarAllocCnt, stackVarFreeCnt, heapObjAllocCnt,
                                 heapObjcFreeCnt,whoInserted);

    char msgz[256];
    if(whoInserted){
      sprintf(msgz,"%s:插入时钟语句,Rwt:%p",whoInserted,&mRewriter);
    }else{
      sprintf(msgz,"插入时钟语句,Rwt:%p",&mRewriter);
    }
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

  /////////////////////////对当前节点compoundStmt做 自定义处理
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
  insertBefore_X__t_clock_tick(mRewriter, insertLoc, stackVarAllocCnt, stackVarFreeCnt, heapObjAllocCnt, heapObjcFreeCnt,"TraverseCompoundStmt");
  }

  ///////////////处理  子语句列表 中每条语句

  for(Stmt* stmt:subStmtLs){
    processStmt(stmt,"TraverseCompoundStmt");
  }
///////////////////// 自定义处理 完毕

////////////////////  将递归链条正确的接好:  对 当前节点compoundStmt 下一层节点stmt们 调用 顶层方法TraverseStmt(stmt)
  for(Stmt* stmt:subStmtLs){
    TraverseStmt  (stmt);
  }

//  Util::printStmt(*Ctx,CI,"查看","组合语句",compoundStmt,false);
  return true;
}


bool CTkVst::TraverseIfStmt(IfStmt *ifStmt){
  /////////////////////////对当前节点compoundStmt做 自定义处理
/*  std::all_of(ifStmt->children().begin(), ifStmt->children().end(),
[this](Stmt* childK){
      Util::printStmt(*this->Ctx,this->CI,"查看VisitIfStmt","的孩子",childK, true);
      return true;
    }
  );*/


  processStmt(ifStmt,"TraverseIfStmt");

///////////////////// 自定义处理 完毕

////////////////////  将递归链条正确的接好:  对 当前节点ifStmt的下一层节点child:{then,else}  调用顶层方法TraverseStmt(child)
  Stmt *thenStmt = ifStmt->getThen();
  Stmt *elseStmt = ifStmt->getElse();

  if(thenStmt){
      TraverseStmt  (thenStmt);
  }

  if(elseStmt){
    TraverseStmt(elseStmt);
  }

  return true;
}
bool CTkVst::TraverseWhileStmt(WhileStmt *whileStmt){
/////////////////////////对当前节点whileStmt做 自定义处理
  processStmt(whileStmt,"TraverseWhileStmt");
///////////////////// 自定义处理 完毕

////////////////////  将递归链条正确的接好:  对 当前节点whileStmt的下一层节点child:{body} 调用顶层方法TraverseStmt(child)
  Stmt *bodyStmt = whileStmt->getBody();
  if(bodyStmt){
    TraverseStmt(bodyStmt);
  }
  return true;
}

bool CTkVst::TraverseForStmt(ForStmt *forStmt) {
/////////////////////////对当前节点forStmt做 自定义处理
  processStmt(forStmt,"TraverseForStmt");
///////////////////// 自定义处理 完毕

////////////////////  将递归链条正确的接好:  对 当前节点forStmt的下一层节点child:{body} 调用顶层方法TraverseStmt(child)
  Stmt *bodyStmt = forStmt->getBody();
  if(bodyStmt){
    TraverseStmt(bodyStmt);
  }
  return true;
}

bool CTkVst::TraverseCXXTryStmt(CXXTryStmt *cxxTryStmt) {

/////////////////////////对当前节点forStmt做 自定义处理
  processStmt(cxxTryStmt,"TraverseCXXTryStmt");
///////////////////// 自定义处理 完毕


////////////////////  将递归链条正确的接好:  对 当前节点cxxTryStmt的下一层节点child:{tryBlock} 调用顶层方法TraverseStmt(child)
  CompoundStmt *tryBlockCompoundStmt = cxxTryStmt->getTryBlock();
  if(tryBlockCompoundStmt){
    TraverseStmt(tryBlockCompoundStmt);
  }
  return true;
}


bool CTkVst::TraverseCXXCatchStmt(CXXCatchStmt *cxxCatchStmt) {

/////////////////////////对当前节点cxxCatchStmt做 自定义处理
  processStmt(cxxCatchStmt,"TraverseCXXCatchStmt");
///////////////////// 自定义处理 完毕

////////////////////  粘接直接子节点到递归链条:  对 当前节点cxxCatchStmt的下一层节点child:{handlerBlock} 调用顶层方法TraverseStmt(child)
  Stmt *handlerBlockStmt = cxxCatchStmt->getHandlerBlock();
  if(handlerBlockStmt){
    TraverseStmt(handlerBlockStmt);
  }
  return true;
}

bool CTkVst::TraverseDoStmt(DoStmt *doStmt) {

/////////////////////////对当前节点doStmt做 自定义处理
  processStmt(doStmt,"TraverseDoStmt");
///////////////////// 自定义处理 完毕

////////////////////  粘接直接子节点到递归链条:  对 当前节点doStmt的下一层节点child:{body} 调用顶层方法TraverseStmt(child)
  Stmt *body = doStmt->getBody();
  if(body){
    TraverseStmt(body);
  }
  return true;
}

bool CTkVst::TraverseSwitchStmt(SwitchStmt *switchStmt) {

/////////////////////////对当前节点switchStmt做 自定义处理
  processStmt(switchStmt,"TraverseSwitchStmt");
///////////////////// 自定义处理 完毕

////////////////////  粘接直接子节点到递归链条:  对 当前节点switchStmt的下一层节点child:{body} 调用顶层方法TraverseStmt(child)
  Stmt *body = switchStmt->getBody();
  if(body){
    TraverseStmt(body);
  }
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
