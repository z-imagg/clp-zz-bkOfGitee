#include "Brc/BrcAstCnsm.h"



bool BrcAstCnsm::mainFileProcessed=false;


std::string BrcAstCnsm::BrcOkFlagText="__BrcOkFlagText";//TODO_ 改为 static

 bool BrcAstCnsm::HandleTopLevelDecl(DeclGroupRef DG) {
  std::vector<Decl*> declVec(DG.begin(),DG.end());

  //如果本文件已处理，则直接返回。
  if(BrcAstCnsm::isProcessed(CI,SM,Ctx,brcOk,declVec)){
    return false;
  }

  return true;
}


//region 判断是否已经处理过了
bool BrcAstCnsm::isProcessed(CompilerInstance& CI,SourceManager&SM, ASTContext& Ctx,  bool& _brcOk, std::vector<Decl*> declVec){
  unsigned long declCnt = declVec.size();
   for(int i=0; i<declCnt; i++){
     Decl* D=declVec[i];
     Util::printDecl(Ctx,CI,"查看声明","",D,true);
     RawComment *rc = Ctx.getRawCommentForDeclNoCache(D);
     //Ctx.getRawCommentForDeclNoCache(D) 获得的注释是完整的
     BrcAstCnsm::__visitRawComment(CI,SM,  rc, _brcOk);
     if(_brcOk){
       std::cout<<"已有标记(已插入花括号),不再处理"<<std::endl;
       return true;
     }
   }
   return false;
}
void BrcAstCnsm::__visitRawComment(CompilerInstance& CI,SourceManager&SM, const RawComment *C, bool & _brcOk) {
  if(!C){
    return;
  }
  const SourceRange &sourceRange = C->getSourceRange();
//  Util::printSourceRangeSimple(CI,"查看RawComment","",sourceRange, true);

  if(_brcOk){
    return;
  }

  LangOptions &langOpts = CI.getLangOpts();
  std::string sourceText = Util::getSourceTextBySourceRange(sourceRange, SM, langOpts);
//      brcOk= (sourceText==BrcOkFlagText);//由于取出来的可能是多个块注释，导致不能用相等判断，只能用下面的包含判断
  std::string::size_type index = sourceText.find(BrcOkFlagText);
  _brcOk= (index != std::string::npos);

}
//endregion
