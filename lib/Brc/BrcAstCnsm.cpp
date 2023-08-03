#include "Brc/BrcAstCnsm.h"



bool BrcAstCnsm::mainFileProcessed=false;


std::string BrcAstCnsm::BrcOkFlagText="__BrcOkFlagText";//TODO_ 改为 static

 bool BrcAstCnsm::HandleTopLevelDecl(DeclGroupRef DG) {
  for (DeclGroupRef::iterator I = DG.begin(), E = DG.end(); I != E; ++I) {
    Decl *D = *I;

    RawComment *rc = Ctx.getRawCommentForDeclNoCache(D);
    //Ctx.getRawCommentForDeclNoCache(D) 获得的注释是完整的
    __visitFullComment(rc, brcOk);
    if(brcOk){
      std::cout<<"已插入花括号,不再处理"<<std::endl;
      return false;
    }
  }

  return true;
}

void BrcAstCnsm::__visitFullComment(const RawComment *C, bool & flag) {
  if(!C){
    return;
  }
  Util::printSourceRangeSimple(CI,"查看RawComment","",C->getSourceRange(), true);

  if(flag){
    return;
  }

  const SourceRange &sourceRange = C->getSourceRange();
  LangOptions &langOpts = CI.getLangOpts();
  std::string sourceText = Util::getSourceTextBySourceRange(sourceRange, SM, langOpts);
//      brcOk= (sourceText==BrcOkFlagText);
  flag= (sourceText.find_first_of(BrcOkFlagText) != std::string::npos);

}