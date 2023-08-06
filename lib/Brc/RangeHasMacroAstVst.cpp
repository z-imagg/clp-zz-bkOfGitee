#include "Brc/RangeHasMacroAstVst.h"



bool RangeHasMacroAstVst::VisitStmt(clang::Stmt *stmt) {

  SourceManager &SM = CI.getSourceManager();

  //如果遍历到的语句的返回 不完全 含在 限定位置范围内 ，不处理，直接返回。 这种情况可能是拿到了一个更大的非终结符号。
//      bool FC = caseKSrcRange.fullyContains(stmt->getSourceRange());//由于 SourceRange::fullyContains 结果是错误的，因此用自制方法Util::fullContains
//      bool FC = CB<=B && CE>=E;//此结果也是错误的.
  bool FC =  Util::fullContains(SM,caseKSrcRange, stmt->getSourceRange());
//  bool FC_=(caseKBL<=sBL) && (caseKEL>=sEL);//这个结果是对的，完善列号即可。 此即 Util::fullContains
  if(!FC){
    return true;///
  }

  caseKSubStmtCnt++;
  Util::printStmt(CI.getASTContext(),CI,"VisitStmt","",stmt,true);

  SourceLocation CB = caseKSrcRange.getBegin();
  SourceLocation CE = caseKSrcRange.getEnd();


  int caseKBL,caseKBC;
  Util::extractLineAndColumn(SM,CB,caseKBL,caseKBC);
  int caseKEL,caseKEC;
  Util::extractLineAndColumn(SM,CE,caseKEL,caseKEC);



  SourceLocation B = stmt->getBeginLoc();
  SourceLocation E = stmt->getEndLoc();
  int sBL,sBC;
  Util::extractLineAndColumn(SM,B,sBL,sBC);
  int sEL,sEC;
  Util::extractLineAndColumn(SM,E,sEL,sEC);

  //如果已经确定 给定Switch的限定位置范围内 有宏，则直接返回，且不需要再遍历了
  if(hasMacro){
    return false;
  }


  //如果遍历到块语句，不处理，直接返回。因为这里只关心单语句，不关心块语句。
  if (clang::isa<clang::CompoundStmt>(stmt)) {
    return true;
  }





  std::string rvAdrr=fmt::format("{:x}",reinterpret_cast<uintptr_t>(this));
//      Util::printStmt(CI.getASTContext(),CI,"xxx",rvAdrr,stmt,true);


  //如果遍历到的单语句，开始位置在宏中 或 结束位置在宏中，则 给定Switch的限定位置范围内 有宏，直接返回，且不需要再遍历了。
  bool inMacro = Util::LocIsInMacro(B,SM) || Util::LocIsInMacro(E,SM);

  if(!hasMacro ){
    if(inMacro){
      hasMacro=true;
      return false;
    }
  }

  //其他情况，继续遍历
  return true;
}