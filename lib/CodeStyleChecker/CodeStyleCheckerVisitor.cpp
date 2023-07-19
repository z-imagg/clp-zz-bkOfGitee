#include "CodeStyleChecker/CodeStyleCheckerVisitor.h"

#include "clang/AST/AST.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"

using namespace clang;

#include <iostream>
#include <clang/AST/ParentMapContext.h>

//-----------------------------------------------------------------------------
// CodeStyleCheckerVisitor implementation
//-----------------------------------------------------------------------------
/*
运行clang++带上本插件.so：但这只是cc1  如何能把整个编译过程串起来？
/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/bin/clang++ -cc1  -load /pubx/clang-tutor/cmake-build-debug/lib/libCodeStyleChecker.so   -plugin CSC   test_main.cpp


只运行了本插件CSC，没有运行编译过程:
#参考: https://releases.llvm.org/8.0.0/tools/clang/docs/ClangPlugins.html
/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/bin/clang++  -Xclang   -load -Xclang /pubx/clang-tutor/cmake-build-debug/lib/libCodeStyleChecker.so  -Xclang   -plugin -Xclang  CSC  -c  /pubx/clang-tutor/test/test_main.cpp

"-plugin" 改为  "-add-plugin", 运行了编译过程:  并输出了 test_main.o
#参考: https://www.ibm.com/docs/en/xl-c-and-cpp-linux/16.1.0?topic=cla-running-user-defined-actions-by-using-clang-plug-ins
/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/bin/clang++  -Xclang   -load -Xclang /pubx/clang-tutor/cmake-build-debug/lib/libCodeStyleChecker.so  -Xclang   -add-plugin -Xclang  CSC  -c  /pubx/clang-tutor/test/test_main.cpp


运行clang++带上本插件.so 且 运行编译、链接 全过程:
/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/bin/clang++  -Xclang   -load -Xclang /pubx/clang-tutor/cmake-build-debug/lib/libCodeStyleChecker.so  -Xclang   -add-plugin -Xclang  CSC   /pubx/clang-tutor/test/test_main.cpp  -o test_main
但运行应用，应用结束时 t没变依然是0，说明本插件对源码的修改没生效.

#更简洁的命令:
运行clang++带上本插件.so 且 运行编译、链接 全过程: 本插件自动运行（ 在MainAction后运行本插件）配合  -fplugin
#本插件自动运行: 在MainAction后运行本插件:  Action.getActionType返回AddAfterMainAction
#参考: https://releases.llvm.org/8.0.0/tools/clang/docs/ClangPlugins.html#using-the-clang-command-line
/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/bin/clang++  -fplugin=/pubx/clang-tutor/cmake-build-debug/lib/libCodeStyleChecker.so     /pubx/clang-tutor/test/test_main.cpp  -o test_main
 */

bool CodeStyleCheckerVisitor::VisitCallExpr(clang::CallExpr *callExpr){

}
bool shouldInsert(clang::Stmt *S,ASTNodeKind& parent0NodeKind){
//clang::Stmt::StmtClass & stmtClass
  clang::Stmt::StmtClass stmtClass = S->getStmtClass();
  switch (stmtClass) {//switch开始
    //{不插入时钟语句概率大的情况
    case clang::Stmt::CompoundStmtClass:{
      return false;
    }
    case clang::Stmt::ImplicitCastExprClass:{ //隐式类型转换
      return false;
    }
    case clang::Stmt::StringLiteralClass:{
      return false;
    }
    case clang::Stmt::IntegerLiteralClass:{
      return false;
    }
    case clang::Stmt::DeclRefExprClass:{//存疑,待找到实例以验证
      return false;
    }
    //}

    //{插入时钟语句概率大的情况
    case clang::Stmt::DeclStmtClass:{
      auto forStmtAstNodeKind=ASTNodeKind::getFromNodeKind<clang::ForStmt>();
      if(parent0NodeKind.isSame(forStmtAstNodeKind) ){
        //如果当前语句S的父亲节点是for语句头，则不插入时钟语句.
        return false;
      }
      return true;
    }
    case clang::Stmt::CallExprClass:{
      {
      ASTNodeKind compoundStmtAstNodeKind=ASTNodeKind::getFromNodeKind<clang::CompoundStmt>();
      if(parent0NodeKind.isSame(compoundStmtAstNodeKind)){
        //如果调用语句CallExpr的父亲节点是组合语句CompoundStmt，则插入时钟语句. 举例如下:
        // 组合语句CompoundStmt: "{ int age; func1();}" , 其中 "func1();" 是调用语句CallExpr.
        return true;
      }
      }

      {
      ASTNodeKind varDeclAstNodeKind=ASTNodeKind::getFromNodeKind<clang::VarDecl>();
      if(parent0NodeKind.isSame(varDeclAstNodeKind)){
        //如果调用语句CallExpr的父亲节点是变量声明语句VarDecl，则不插入时钟语句. 举例如下:
        // 变量声明语句VarDecl: "float cost=calcCost(3,false);" , 其中 "calcCost(3,false)" 是调用语句CallExpr.
        return false;
      }
      }

      {
      ASTNodeKind binaryOperatorAstNodeKind=ASTNodeKind::getFromNodeKind<clang::BinaryOperator>();
      if(parent0NodeKind.isSame(binaryOperatorAstNodeKind)){
        //如果调用语句CallExpr的父亲节点是 二元操作符BinaryOperator，则不插入时钟语句. 举例如下:
        // 二元操作符BinaryOperator 语句: "name=getName(true);" , 其中 "getName(true)" 是调用语句CallExpr, 二元操作符BinaryOperator 这里是指 赋值号"="
        return false;
      }
      }

      {
      ASTNodeKind ifStmtAstNodeKind=ASTNodeKind::getFromNodeKind<clang::IfStmt>();
      if(parent0NodeKind.isSame(ifStmtAstNodeKind)){
        //如果调用语句CallExpr的父亲节点是 判断语句IfStmt，则不插入时钟语句，否则破坏了原来意思. 举例如下:
        // 判断语句IfStmt  : "if(done) do_job();" , 其中 "do_job()" 是调用语句CallExpr
        return false;
      }
      }

      return true;
    }
    case clang::Stmt::ForStmtClass: {//for循环整体
      return true;
    }
//    case clang::Stmt::UnaryOperatorClass://一元操作符语句,  这里 暂时不插入. 因为需要知道当前是在for循环第3位置 , 还是单独一条语句. for循环第3位置前插入 分割符只能是逗号, 单独一条语句前插入 分割符只能是分号.
//    case clang::Stmt::BinaryOperatorClass://二元操作符语句，暂时不插入。 需要看其内部组成和外部包裹，比如若是被if()包裹 肯定其前肯定不能插入，如果是单独一条语句 其前可以插入。
    case clang::Stmt::IfStmtClass: {//if语句整体
      return true;
    }
    case clang::Stmt::ReturnStmtClass:{
      return true;
    }
    //}
  }//switch结束


  //默认不插入
  return false;
}

/**遍历语句
 *
 * @param S
 * @return
 */
bool CodeStyleCheckerVisitor::VisitStmt(clang::Stmt *S){
  //ref:  https://stackoverflow.com/questions/40596195/pretty-print-statement-to-string-in-clang/40599057#40599057
  auto range=S->getSourceRange();
  auto cRange=CharSourceRange::getCharRange(range);
  llvm::StringRef strRefStmt=Lexer::getSourceText(cRange, mRewriter.getSourceMgr(), mRewriter.getLangOpts());

  auto strStmt=strRefStmt.str();

  auto stmtClass = S->getStmtClass();
  auto stmtClassName = S->getStmtClassName();


//  std::cout << "[#" << strStmt << "#]:{#" << stmtClassName << "#}" ;  //开发用打印

  clang::DynTypedNodeList parentS=this->Ctx->getParents(*S);
  auto parentSSize=parentS.size();
  assert(parentSSize>0);
//  if(parentSSize>0){
    auto parent0=parentS[0].get<Stmt>();
    ASTNodeKind parent0NodeKind=parentS[0].getNodeKind();
    auto end=true;

//    std::cout << parent0NodeKind.asStringRef().str() << std::endl;  //开发用打印
//  }else{
//    std::cout << std::endl;  //开发用打印
//  }
  bool isCompoundStmtClass= ( stmtClass==clang::Stmt::CompoundStmtClass);
  if(shouldInsert(S,parent0NodeKind)){
//  mRewriter.InsertTextAfter(S->getEndLoc(),"/**/");
    mRewriter.InsertTextBefore(S->getBeginLoc(),"t++;");
  }
  return true;
}

/*
[#{
  int age;
  printf("input age:");
  scanf("%d",&age);
  printf("your age:%d\n",age);

  int alive=false;
  int secret[100];
  for(int k =0; k <100; k++){
    if (secret[k] < age){
      alive=true;
      break;
    }
  }

  printf("are you still be alive?%d\n",alive);

  return 0;

#]:{#CompoundStmt#}
[#int age#]:{#DeclStmt#}
[#printf("input age:"#]:{#CallExpr#}
[##]:{#ImplicitCastExpr#}
[##]:{#DeclRefExpr#}
[##]:{#ImplicitCastExpr#}
[##]:{#StringLiteral#}
[#scanf("%d",&age#]:{#CallExpr#}
[##]:{#ImplicitCastExpr#}
[##]:{#DeclRefExpr#}
[##]:{#ImplicitCastExpr#}
[##]:{#StringLiteral#}
[#&#]:{#UnaryOperator#}
[##]:{#DeclRefExpr#}
[#printf("your age:%d\n",age#]:{#CallExpr#}
[##]:{#ImplicitCastExpr#}
[##]:{#DeclRefExpr#}
[##]:{#ImplicitCastExpr#}
[##]:{#StringLiteral#}
[##]:{#ImplicitCastExpr#}
[##]:{#DeclRefExpr#}
[#int alive=false#]:{#DeclStmt#}
[##]:{#ImplicitCastExpr#}
[##]:{#CXXBoolLiteralExpr#}
[#int secret[100]#]:{#DeclStmt#}
[##]:{#IntegerLiteral#}
[#for(int k =0; k <100; k++){
    if (secret[k] < age){
      alive=true;
      break;
    }
  #]:{#ForStmt#}
[#int k =0#]:{#DeclStmt#}
[##]:{#IntegerLiteral#}
[#k <#]:{#BinaryOperator#}
[##]:{#ImplicitCastExpr#}
[##]:{#DeclRefExpr#}
[##]:{#IntegerLiteral#}
[#k#]:{#UnaryOperator#}
[##]:{#DeclRefExpr#}
[#{
    if (secret[k] < age){
      alive=true;
      break;
    }
  #]:{#CompoundStmt#}
[#if (secret[k] < age){
      alive=true;
      break;
    #]:{#IfStmt#}
[#secret[k] < #]:{#BinaryOperator#}
[#secret[k#]:{#ImplicitCastExpr#}
[#secret[k#]:{#ArraySubscriptExpr#}
[##]:{#ImplicitCastExpr#}
[##]:{#DeclRefExpr#}
[##]:{#ImplicitCastExpr#}
[##]:{#DeclRefExpr#}
[##]:{#ImplicitCastExpr#}
[##]:{#DeclRefExpr#}
[#{
      alive=true;
      break;
    #]:{#CompoundStmt#}
[#alive=#]:{#BinaryOperator#}
[##]:{#DeclRefExpr#}
[##]:{#ImplicitCastExpr#}
[##]:{#CXXBoolLiteralExpr#}
[##]:{#BreakStmt#}
[#printf("are you still be alive?%d\n",alive#]:{#CallExpr#}
[##]:{#ImplicitCastExpr#}
[##]:{#DeclRefExpr#}
[##]:{#ImplicitCastExpr#}
[##]:{#StringLiteral#}
[##]:{#ImplicitCastExpr#}
[##]:{#DeclRefExpr#}
[#return #]:{#ReturnStmt#}
[##]:{#IntegerLiteral#}
 */


bool CodeStyleCheckerVisitor::VisitCXXRecordDecl(CXXRecordDecl *Decl) {
  // Skip anonymous records, e.g. unions:
  //    * https://en.cppreference.com/w/cpp/language/union
  if (0 == Decl->getNameAsString().size())
    return true;

  checkNameStartsWithUpperCase(Decl);
  checkNoUnderscoreInName(Decl);
  return true;
}

bool CodeStyleCheckerVisitor::VisitFunctionDecl(FunctionDecl *Decl) {
  // Skip user-defined conversion operators/functions:
  //    * https://en.cppreference.com/w/cpp/language/cast_operator
  if (isa<CXXConversionDecl>(Decl))
    return true;

  checkNameStartsWithLowerCase(Decl);
  checkNoUnderscoreInName(Decl);
  return true;
}

bool CodeStyleCheckerVisitor::VisitVarDecl(VarDecl *Decl) {
  // Skip anonymous function parameter declarations
  if (isa<ParmVarDecl>(Decl) && (0 == Decl->getNameAsString().size()))
    return true;

  checkNameStartsWithUpperCase(Decl);
  checkNoUnderscoreInName(Decl);
  return true;
}

bool CodeStyleCheckerVisitor::VisitFieldDecl(FieldDecl *Decl) {
  // Skip anonymous bit-fields:
  //  * https://en.cppreference.com/w/c/language/bit_field
  if (0 == Decl->getNameAsString().size())
    return true;

  checkNameStartsWithUpperCase(Decl);
  checkNoUnderscoreInName(Decl);

  return true;
}

void CodeStyleCheckerVisitor::checkNoUnderscoreInName(NamedDecl *Decl) {
  auto Name = Decl->getNameAsString();
  size_t underscorePos = Name.find('_');

  if (underscorePos == StringRef::npos)
    return;

  std::string Hint = Name;
  auto end_pos = std::remove(Hint.begin(), Hint.end(), '_');
  Hint.erase(end_pos, Hint.end());

  FixItHint FixItHint = FixItHint::CreateReplacement(
          SourceRange(Decl->getLocation(),
                      Decl->getLocation().getLocWithOffset(Name.size())),
          Hint);

  DiagnosticsEngine &DiagEngine = Ctx->getDiagnostics();
  unsigned DiagID = DiagEngine.getCustomDiagID(DiagnosticsEngine::Warning,
                                               "`_` in names is not allowed");
  SourceLocation UnderscoreLoc =
          Decl->getLocation().getLocWithOffset(underscorePos);
  DiagEngine.Report(UnderscoreLoc, DiagID).AddFixItHint(FixItHint);
}

void CodeStyleCheckerVisitor::checkNameStartsWithLowerCase(NamedDecl *Decl) {
  auto Name = Decl->getNameAsString();
  char FirstChar = Name[0];

  // The actual check
  if (isLowercase(FirstChar))
    return;

  // Construct the hint
  std::string Hint = Name;
  Hint[0] = toLowercase(FirstChar);
  FixItHint FixItHint = FixItHint::CreateReplacement(
          SourceRange(Decl->getLocation(),
                      Decl->getLocation().getLocWithOffset(Name.size())),
          Hint);

  // Construct the diagnostic
  DiagnosticsEngine &DiagEngine = Ctx->getDiagnostics();
  unsigned DiagID = DiagEngine.getCustomDiagID(
          DiagnosticsEngine::Warning,
          "Function names should start with lower-case letter");
  DiagEngine.Report(Decl->getLocation(), DiagID) << FixItHint;
}

void CodeStyleCheckerVisitor::checkNameStartsWithUpperCase(NamedDecl *Decl) {
  auto Name = Decl->getNameAsString();
  char FirstChar = Name[0];

  // The actual check
  if (isUppercase(FirstChar))
    return;

  // Construct the hint
  std::string Hint = Name;
  Hint[0] = toUppercase(FirstChar);
  FixItHint FixItHint = FixItHint::CreateReplacement(
          SourceRange(Decl->getLocation(),
                      Decl->getLocation().getLocWithOffset(Name.size())),
          Hint);

  // Construct the diagnostic
  DiagnosticsEngine &DiagEngine = Ctx->getDiagnostics();
  unsigned DiagID = DiagEngine.getCustomDiagID(
          DiagnosticsEngine::Warning,
          "Type and variable names should start with upper-case letter");
  DiagEngine.Report(Decl->getLocation(), DiagID) << FixItHint;
}
