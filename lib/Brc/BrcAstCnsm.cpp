#include "Brc/BrcAstCnsm.h"



bool BrcAstCnsm::mainFileProcessed=false;


std::string BrcAstCnsm::BrcOkFlagText="__BrcOkFlagText";//TODO_ 改为 static

 bool BrcAstCnsm::HandleTopLevelDecl(DeclGroupRef DG) {
  for (DeclGroupRef::iterator I = DG.begin(), E = DG.end(); I != E; ++I) {
    Decl *D = *I;

    RawComment *rc = Ctx.getRawCommentForDeclNoCache(D);
    //Ctx.getRawCommentForDeclNoCache(D) 获得的注释是完整的
    __visitRawComment(rc, brcOk);
    if(brcOk){
      std::cout<<"已插入花括号,不再处理"<<std::endl;
      return false;
    }
  }

  return true;
}

void BrcAstCnsm::__visitRawComment(const RawComment *C, bool & _brcOk) {
  if(!C){
    return;
  }
  const SourceRange &sourceRange = C->getSourceRange();
  Util::printSourceRangeSimple(CI,"查看RawComment","",sourceRange, true);

  if(_brcOk){
    return;
  }

  LangOptions &langOpts = CI.getLangOpts();
  std::string sourceText = Util::getSourceTextBySourceRange(sourceRange, SM, langOpts);
//      brcOk= (sourceText==BrcOkFlagText);//由于取出来的可能是多个块注释，导致不能用相等判断，只能用下面的包含判断
  _brcOk= (sourceText.find_first_of(BrcOkFlagText) != std::string::npos);

}