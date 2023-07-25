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


利用  运行clang++的编译 带上本插件.so  实现 对源文件插入时钟滴答语句:
/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/bin/clang++  -Xclang   -load -Xclang /pubx/clang-tutor/cmake-build-debug/lib/libCTk.so  -Xclang   -add-plugin -Xclang  CTk  -c /pubx/clang-tutor/t_clock_tick/test_main.cpp


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










/**给定源文件路径是否系统源文件
 * 系统源文件路径举例：
/usr/lib/gcc/x86_64-linux-gnu/11/../../../../include/c++/11/bits/cpp_type_traits.h
/usr/lib/gcc/x86_64-linux-gnu/11/../../../../include/c++/11/ext/type_traits.h
/usr/include/x86_64-linux-gnu/bits/iscanonical.h

/app/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/lib/clang/15.0.0/include/uintrintrin.h
 * @param fn
 * @return
 */
bool CTkVst::isInternalSysSourceFile(StringRef fn) {
  bool startWithUsr=fn.startswith("/usr/");
  bool isLLVM01=fn.startswith("/app/llvm_release_home/clang+llvm");
  bool isLLVM02=fn.startswith("/llvm_release_home/clang+llvm");
  bool isInternal=(startWithUsr||isLLVM01||isLLVM02);
  return isInternal;
}

void CTkVst::insertBefore_X__t_clock_tick(LifeStep lifeStep, int64_t stmtId, SourceLocation stmtBeginLoc, int stackVarAllocCnt, int stackVarFreeCnt, int heapObjAllocCnt, int heapObjcFreeCnt, const char* whoInserted){
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
  mRewriter_ptr->InsertTextBefore(stmtBeginLoc, strRef_X__t_clock_tick);//B.   B处mRewriter和A处mRewriter 地址相同，但A处mRewriter.SourceMgr非空，B处mRewriter为空。

  //记录已插入语句的节点ID们以防重： 即使重复遍历了 但不会重复插入
  if(lifeStep == LifeStep::Alloc){
    allocInsertedNodeIDLs.insert(stmtId);
  }else if(lifeStep == LifeStep::Free){
    freeInsertedNodeIDLs.insert(stmtId);
  }
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
  this->mRewriter_ptr->setSourceMgr(this->SM,CI.getLangOpts());

  int64_t stmtId = stmt->getID(*Ctx);

  if(allocInsertedNodeIDLs.count(stmtId) > 0){
    //如果 本节点ID 已经被插入语句，则不必插入，直接返回即可。
    //依据已插入语句的节点ID们可防重： 即使此次是重复的遍历， 但不会重复插入
    return false;
  }

//  SourceManager & SM = mRewriter.getSourceMgr();//此处的mRewriter的SourceMgr是空, 所以才会有C处崩溃，因为C处用 NULL.getFileID() 肯定崩溃。
  const LangOptions & langOpts = CI.getLangOpts();


  SourceLocation beginLoc=stmt->getBeginLoc();
  SourceRange sourceRange=stmt->getSourceRange();
  FileID fileId = SM.getFileID(beginLoc);//C

  FileID mainFileId = SM.getMainFileID();

  std::string stmtFileAndRange=sourceRange.printToString(SM);

  //获取当前语句S的源码文本
  std::string stmtSourceText=Util::getSourceTextBySourceRange(stmt->getSourceRange(), SM, langOpts);

///////若某函数 有 constexpr 修饰，则在TraverseCXXMethodDecl|TraverseFunctionDecl中被拒绝 粘接直接子节点到递归链条 ，这样该函数体 无法   经过 TraverseStmt(函数体) ---...--->TraverseCompoundStmt(函数体) 转交， 即   不可能 有  TraverseCompoundStmt(该函数体) ， 即  该该函数体中的每条子语句前都 不会 有机会 被  插入 时钟调用语句.

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

  char msg[256];
  sprintf(msg,"parent0NodeKind:%s,_isInternalSysSourceFile:%d",parent0NodeKindCStr,_isInternalSysSourceFile);//sprintf中不要给 clang::StringRef类型，否则结果是怪异的。
  Util::printStmt(*Ctx, CI, "查看_VisitStmt", msg, stmt, true);  //开发用打印

  if( ( !_isInternalSysSourceFile )){

//    stmtClass=stmt->getStmtClass();
    int stackVarAllocCnt=0;
    int stackVarFreeCnt=0;
    int heapObjAllocCnt=0;
    int heapObjcFreeCnt=0;
    if(stmtClass==Stmt::StmtClass::DeclStmtClass){
      //如果当前语句是声明语句
      DeclStmt *declStmt = static_cast<DeclStmt *>(stmt);
      //取得声明语句declStmt 中声明的变量个数. 比如 声明语句"int x=0,y;"中声明了2个变量
      stackVarAllocCnt=Util::varCntInVarDecl(declStmt);
    }
    insertBefore_X__t_clock_tick(LifeStep::Alloc, stmtId, stmt->getBeginLoc(), stackVarAllocCnt, stackVarFreeCnt, heapObjAllocCnt,
                                 heapObjcFreeCnt, whoInserted);



    char msgz[256];
    if(whoInserted){
      sprintf(msgz,"%s:插入时钟语句,Rwt:%p",whoInserted,mRewriter_ptr);
    }else{
      sprintf(msgz,"插入时钟语句,Rwt:%p",mRewriter_ptr);
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
  int64_t compoundStmtID = compoundStmt->getID(*Ctx);
  const Stmt::child_range &subStmtLs = compoundStmt->children();

  ///////////////计算 子语句列表 中 变量声明语句个数，以生成释放语句 并插入
  //此组合语句内的变量声明语句个数
  int declStmtCnt=0;

  for(Stmt* subStmt:subStmtLs){
    const char *subStmtClassName = subStmt->getStmtClassName();
    Stmt::StmtClass subStmtClass = subStmt->getStmtClass();
    if(Stmt::DeclStmtClass == subStmtClass){
      DeclStmt* declStmt=static_cast<DeclStmt*> (subStmt);
      //取得声明语句subDeclStmt 中声明的变量个数. 比如 声明语句"int x=0,y;"中声明了2个变量
      declStmtCnt+=Util::varCntInVarDecl(declStmt);
    }
//    Util::printStmt(*Ctx,CI,"查看组合语句内子语句类型","",subStmt,true);
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

  //释放语句 未曾插入过吗？
  bool freeNotInserted=freeInsertedNodeIDLs.count(compoundStmtID) <= 0;
  //若 有 栈变量释放 且 未曾插入过 释放语句，则插入释放语句
  if(declStmtCnt>0 && freeNotInserted){
  int stackVarAllocCnt=0;
  int stackVarFreeCnt=declStmtCnt;
  int heapObjAllocCnt=0;
  int heapObjcFreeCnt=0;
  insertBefore_X__t_clock_tick(LifeStep::Free, compoundStmtID, insertLoc, stackVarAllocCnt, stackVarFreeCnt, heapObjAllocCnt, heapObjcFreeCnt, "TraverseCompoundStmt");
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

  if(ifStmt){
    if(Util::parentIsCompound(Ctx,ifStmt)){
      processStmt(ifStmt,"TraverseIfStmt");
    }
  }

///////////////////// 自定义处理 完毕

////////////////////  将递归链条正确的接好:  对 当前节点ifStmt的下一层节点child:{then,else}  调用顶层方法TraverseStmt(child)
  Stmt *thenStmt = ifStmt->getThen();
  Stmt *elseStmt = ifStmt->getElse();

  if(thenStmt){
    Stmt::StmtClass thenStmtClass = thenStmt->getStmtClass();
//    if(thenStmtClass==Stmt::StmtClass::CompoundStmtClass){
      //这一段可以替代shouldInsert
      /**只有当if的then子语句是 块语句 时, 该 then子语句，才需要 经过 TraverseStmt(thenStmt) ---...--->TraverseCompoundStmt(thenStmt) 转交，在 TraverseCompoundStmt(thenStmt) 中 对 then块中的每条子语句前 插入 时钟调用语句.
       * 形如:
       * if(...)
       * {
       * ...;//这里是 if的then子语句, 是一个块语句，需要 对 then块中的每条子语句前 插入 时钟调用语句.
       * }
       */
      TraverseStmt  (thenStmt);
//    }
    /**否则 if的then子语句 肯定是一个单行语句，无需插入 时钟调用语句.
     * 形如 :
     * if(...)
     *   ...;// 这里是 if的then子语句, 是一个单行语句，无需插入 时钟调用语句.
     */
  }

  if(elseStmt){
    TraverseStmt(elseStmt);
  }

  return true;
}
bool CTkVst::TraverseWhileStmt(WhileStmt *whileStmt){
/////////////////////////对当前节点whileStmt做 自定义处理
  if(Util::parentIsCompound(Ctx,whileStmt)){
    processStmt(whileStmt,"TraverseWhileStmt");
  }
///////////////////// 自定义处理 完毕

////////////////////  将递归链条正确的接好:  对 当前节点whileStmt的下一层节点child:{body} 调用顶层方法TraverseStmt(child)
  Stmt *bodyStmt = whileStmt->getBody();
  if(bodyStmt){

    Stmt::StmtClass bodyStmtClass = bodyStmt->getStmtClass();
    if(bodyStmtClass==Stmt::StmtClass::CompoundStmtClass){
      //这一段可以替代shouldInsert
      /**只有当while的循环体是 块语句 时, 该 循环体，才需要 经过 TraverseStmt(循环体) ---...--->TraverseCompoundStmt(循环体) 转交，在 TraverseCompoundStmt(循环体) 中 对 该循环体中的每条子语句前 插入 时钟调用语句.
       * 形如:
       * while(...)
       * {
       * ...;//这里是 while的循环体, 是一个块语句，需要 对 循环体中的每条子语句前 插入 时钟调用语句.
       * }
       */
      TraverseStmt(bodyStmt);
    }
    /**否则 while的循环体 肯定是一个单行语句，无需插入 时钟调用语句.
     * 形如 :
     * while(...)
     *   ...;// 这里是 while的循环体, 是一个单行语句，无需插入 时钟调用语句.
     */
  }
  return true;
}

bool CTkVst::TraverseForStmt(ForStmt *forStmt) {
/////////////////////////对当前节点forStmt做 自定义处理
  if(Util::parentIsCompound(Ctx,forStmt)){
    processStmt(forStmt,"TraverseForStmt");
  }
///////////////////// 自定义处理 完毕

////////////////////  将递归链条正确的接好:  对 当前节点forStmt的下一层节点child:{body} 调用顶层方法TraverseStmt(child)
  Stmt *bodyStmt = forStmt->getBody();
  if(bodyStmt){
    Stmt::StmtClass bodyStmtClass = bodyStmt->getStmtClass();
//    if(bodyStmtClass==Stmt::StmtClass::CompoundStmtClass){
      //这一段可以替代shouldInsert
      TraverseStmt(bodyStmt);
//    }
  }
  return true;
}

bool CTkVst::TraverseCXXTryStmt(CXXTryStmt *cxxTryStmt) {

/////////////////////////对当前节点forStmt做 自定义处理
  if(Util::parentIsCompound(Ctx,cxxTryStmt)){
    processStmt(cxxTryStmt,"TraverseCXXTryStmt");
  }
///////////////////// 自定义处理 完毕


////////////////////  将递归链条正确的接好:  对 当前节点cxxTryStmt的下一层节点child:{tryBlock} 调用顶层方法TraverseStmt(child)
  CompoundStmt *tryBlockCompoundStmt = cxxTryStmt->getTryBlock();
  if(tryBlockCompoundStmt){

    Stmt::StmtClass tryBlockCompoundStmtClass = tryBlockCompoundStmt->getStmtClass();
    assert(tryBlockCompoundStmtClass==Stmt::StmtClass::CompoundStmtClass) ;//C++Try的尝试体一定是块语句

    TraverseStmt(tryBlockCompoundStmt);
  }
  return true;
}


bool CTkVst::TraverseCXXCatchStmt(CXXCatchStmt *cxxCatchStmt) {

/////////////////////////对当前节点cxxCatchStmt做 自定义处理
//  processStmt(cxxCatchStmt,"TraverseCXXCatchStmt");//catch整体 前 肯定不能插入
///////////////////// 自定义处理 完毕

////////////////////  粘接直接子节点到递归链条:  对 当前节点cxxCatchStmt的下一层节点child:{handlerBlock} 调用顶层方法TraverseStmt(child)
  Stmt *handlerBlockStmt = cxxCatchStmt->getHandlerBlock();
  if(handlerBlockStmt){
    Stmt::StmtClass handlerBlockStmtClass = handlerBlockStmt->getStmtClass();
    assert(handlerBlockStmtClass==Stmt::StmtClass::CompoundStmtClass) ;//C++Catch的捕捉体一定是块语句

    TraverseStmt(handlerBlockStmt);
  }
  return true;
}

bool CTkVst::TraverseDoStmt(DoStmt *doStmt) {

/////////////////////////对当前节点doStmt做 自定义处理
  if(Util::parentIsCompound(Ctx,doStmt)){
    processStmt(doStmt,"TraverseDoStmt");
  }
///////////////////// 自定义处理 完毕

////////////////////  粘接直接子节点到递归链条:  对 当前节点doStmt的下一层节点child:{body} 调用顶层方法TraverseStmt(child)
  Stmt *body = doStmt->getBody();
  if(body){
    Stmt::StmtClass bodyStmtClass = body->getStmtClass();
    if(bodyStmtClass==Stmt::StmtClass::CompoundStmtClass){
      //这一段可以替代shouldInsert
      TraverseStmt(body);
    }
  }
  return true;
}

bool CTkVst::TraverseSwitchStmt(SwitchStmt *switchStmt) {
//switchStmt: switch整体:  'switch(v){ case k1:{...}  case k2:{...}  default:{} }'
/////////////////////////对当前节点switchStmt做 自定义处理
  if(Util::parentIsCompound(Ctx,switchStmt)){
    processStmt(switchStmt,"TraverseSwitchStmt");
  }
///////////////////// 自定义处理 完毕

////////////////////  粘接直接子节点到递归链条:  对 当前节点switchStmt的下一层节点child:{switchBody} 调用顶层方法TraverseStmt(child)
  Stmt *switchBody = switchStmt->getBody();//switchBody：switch体 : 即   '{ case k1:{...}  case k2:{...}  default:{} }'
  if(!switchBody){
    return true;
  }

  Stmt::StmtClass bodyStmtClass = switchBody->getStmtClass();
  assert(bodyStmtClass==Stmt::StmtClass::CompoundStmtClass) ;//switch语句的多情况体 一定是块语句
  //不要遍历switch体 即 不要有 TraverseStmt(switch体), 否则会有 TraverseCompoundStmt(switch体) 即 : 会在case前插入语句，这是错误的。

  for (SwitchCase *switchCaseK = switchStmt->getSwitchCaseList(); switchCaseK != nullptr; switchCaseK = switchCaseK->getNextSwitchCase()) {
    if (CaseStmt *caseKStmt = dyn_cast<CaseStmt>(switchCaseK)) {
      //caseKStmt: case k 整体 : 'case k: {....}'
      Stmt *caseKBody = caseKStmt->getSubStmt();//caseKBody: case k 的 身体 : 即 'case k: {...}'  中的 '{...}'
      if(caseKBody){
        TraverseStmt(caseKBody);
      }
    }
  }

  return true;
}


bool CTkVst::TraverseCaseStmt(CaseStmt *caseStmt) {

/////////////////////////对当前节点caseStmt 不做 自定义处理
///////////////////// 自定义处理 完毕

////////////////////  粘接直接子节点到递归链条:  对 当前节点caseStmt的下一层节点中的单情况体  调用顶层方法TraverseStmt(单情况体)
                                /////case的其他子节点，比如 'case 值:' 中的 '值' 不做处理。
  Stmt *body = caseStmt->getSubStmt();//不太确定 case.getSubStmt 是 获取case的单情况体
  if(body){
    Stmt::StmtClass bodyStmtClass = body->getStmtClass();
    if(bodyStmtClass==Stmt::StmtClass::CompoundStmtClass){
      //这一段可以替代shouldInsert
      TraverseStmt(body);
    }
  }
  return true;
}


////////////////constexpr

bool CTkVst::TraverseFunctionDecl(FunctionDecl *functionDecl) {
  const SourceRange &sourceRange = functionDecl->getSourceRange();
  bool _isConstexpr = functionDecl->isConstexpr();
  Stmt *body = functionDecl->getBody();

  return this->_Traverse_Func(sourceRange,_isConstexpr,body);
}

bool CTkVst::TraverseCXXConstructorDecl(CXXConstructorDecl* cxxConstructorDecl){
  const SourceRange &sourceRange = cxxConstructorDecl->getSourceRange();
  bool _isConstexpr = cxxConstructorDecl->isConstexpr();
  Stmt *body = cxxConstructorDecl->getBody();

  return this->_Traverse_Func(sourceRange,_isConstexpr,body);
}

bool CTkVst::TraverseCXXMethodDecl(CXXMethodDecl* cxxMethodDecl){
  const SourceRange &sourceRange = cxxMethodDecl->getSourceRange();
  bool _isConstexpr = cxxMethodDecl->isConstexpr();
  Stmt *body = cxxMethodDecl->getBody();

  return this->_Traverse_Func(sourceRange,_isConstexpr,body);
}

bool CTkVst::_Traverse_Func(
        const SourceRange &funcSourceRange,
        bool funcIsConstexpr,
        Stmt *funcBodyStmt
){

/////////////////////////对当前节点cxxMethodDecl|functionDecl做 自定义处理

  const SourceRange &sourceRange = funcSourceRange;

//TODO 函数体 左花括号{ 后插入语句， 栈变量分配个数 为函数参数个数
//TODO 函数体 右边花括号} 前插入语句， 栈变量释放个数 为
// 函数参数个数 A
// +
// 函数体内栈变量分配个数 B :
//    （不包括函数体内嵌套的语句块内的栈变量分配个数）
// B部分在 函数体 作为 组合语句 那算过了, 简单解决 就是不解决 ： 让A占据一条 插入语句 B也占据一条插入语句 ，只是滴答数多了1次（没多大影响），  栈变量分配个数  还是A+B  没问题

  bool _isConstexpr = funcIsConstexpr;
///////////////////// 自定义处理 完毕

////////////////////  粘接直接子节点到递归链条:  对 当前节点cxxMethodDecl|functionDecl的下一层节点child:{body} 调用顶层方法TraverseStmt(child)
  Stmt *body = funcBodyStmt;
  if(!_isConstexpr){
    //若此函数 有 constexpr 修饰，则拒绝 粘接直接子节点到递归链条 ，这样该函数体 无法   经过 TraverseStmt(函数体) ---...--->TraverseCompoundStmt(函数体) 转交， 即   不可能 有  TraverseCompoundStmt(该函数体) ， 即  该该函数体中的每条子语句前都 不会 有机会 被  插入 时钟调用语句.
    //    由此 变相实现了  constexpr_func_ls标记, 因此 constexpr_func_ls标记可以删除了.
    if(body){
      TraverseStmt(body);
    }
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
