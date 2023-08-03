#include "Brc/BrcAstCnsm.h"



bool BrcAstCnsm::mainFileProcessed=false;


std::string BrcAstCnsm::BrcOkFlagText="__BrcOkFlagText";

 bool BrcAstCnsm::HandleTopLevelDecl(DeclGroupRef DG) {


   //region 获取主文件ID，文件路径
   FileID mainFileId;
   std::string filePath;
   Util::getMainFileIDMainFilePath(SM,mainFileId,filePath);
   //endregion

   //region 声明组转为声明vector
  std::vector<Decl*> declVec(DG.begin(),DG.end());
  //endregion

  //region 1.若本文件已处理，则直接返回。
  if(BrcAstCnsm::isProcessed(CI,SM,Ctx,brcOk,declVec)){
    return false;
  }
  //endregion

  //region 2. 插入花括号
   unsigned long declCnt = declVec.size();
   for(int i=0; i<declCnt; i++) {
     Decl *D = declVec[i];
     this->brcVst.TraverseDecl(D);
   }
  //endregion

  //region 3. 插入已处理标记
   bool insertResult;
   //插入的注释语句不要带换行,这样不破坏原行号
   const std::string brcOkFlagComment = fmt::format("/*{}*/", BrcOkFlagText);
   Util::insertIncludeToFileStart(brcOkFlagComment, mainFileId, SM, brcVst.mRewriter_ptr, insertResult);
  //endregion

   //region 4. 应用修改到源文件
   brcVst.mRewriter_ptr->overwriteChangedFiles();
//   DiagnosticsEngine &de = SM.getDiagnostics();//de是空的，没有DiagnosticsEngine?
   DiagnosticsEngine &de = CI.getDiagnostics();//
   DiagnosticsEngine &Diags = CI.getDiagnostics();
   int error=Diags.getNumErrors();
   bool hasErrorOccurred = Diags.hasErrorOccurred();
   bool hasFatalErrorOccurred = Diags.hasFatalErrorOccurred();
   bool hasUncompilableErrorOccurred = Diags.hasUncompilableErrorOccurred();
   bool hasUnrecoverableErrorOccurred = Diags.hasUnrecoverableErrorOccurred();
   // 检查是否有错误发生
//   if (Diags.hasErrorOccurred()) {
//     // 遍历诊断信息，输出错误信息
//     DiagnosticIDs *xx = Diags.getDiagnosticIDs().get();
//     for (unsigned int i = 0; i < Diags.getDiagnosticIDs()->getNumBuiltinDiagnostics(); ++i) {
//       const auto& DiagInfo = Diags.getDiagnosticIDs()->getBuiltinDiagnostic(i);
//       if (DiagInfo.getLevel() == DiagnosticIDs::Error) {
//         // 输出错误信息
//         Diags.getDiagnostic(DiagInfo.getID()).getLocation().print(llvm::outs(), CI.getSourceManager());
//         llvm::outs() << ": " << Diags.getDiagnostic(DiagInfo.getID()).getMessage() << "\n";
//       }
//     }
//   }
   //endregion
  return true;
}


//region 判断是否已经处理过了
bool BrcAstCnsm::isProcessed(CompilerInstance& CI,SourceManager&SM, ASTContext& Ctx,  bool& _brcOk, std::vector<Decl*> declVec){
  unsigned long declCnt = declVec.size();
   for(int i=0; i<declCnt; i++){
     Decl* D=declVec[i];

     //忽略非主文件中的声明
     if(!Util::isDeclInMainFile(SM,D)){
       continue;
     }

//     Util::printDecl(Ctx,CI,"查看声明","",D,true);
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
