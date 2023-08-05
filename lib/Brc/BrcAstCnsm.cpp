#include "Brc/BrcAstCnsm.h"



bool BrcAstCnsm::mainFileProcessed=false;


std::string BrcAstCnsm::BrcOkFlagText="__BrcOkFlagText";

 void BrcAstCnsm::HandleTranslationUnit(ASTContext &Ctx) {
   std::cout<< fmt::format("HandleTranslationUnit打印各重要对象地址: CI:{:x},this->Ctx:{:x},Ctx:{:x},SM:{:x},mRewriter_ptr:{:x}",
reinterpret_cast<uintptr_t> (&CI ),
reinterpret_cast<uintptr_t> (&(this->Ctx) ),
reinterpret_cast<uintptr_t> (&Ctx ),
reinterpret_cast<uintptr_t> (&SM ),
reinterpret_cast<uintptr_t> ( (brcVst.mRewriter_ptr.get()) ) ) <<std::endl;
//  ASTConsumer::HandleTranslationUnit(Ctx);

   TranslationUnitDecl *translationUnitDecl = Ctx.getTranslationUnitDecl();


   //region 顶层翻译单元 的 声明们 既有 主文件的 又有 非主文件的，故 在顶层 不合适 做 跳过 非主文件
   //  translationUnitDecl中同时包含 非MainFile中的Decl、MainFile中的Decl
   //    因此不能用translationUnitDecl的位置 判断当前是否在MainFile中
//  if(!Util::isDeclInMainFile(SM,translationUnitDecl)){
//    return;
//  }
  //endregion

   //region 声明组转为声明vector
   const DeclContext::decl_range &Decls = translationUnitDecl->decls();
   std::vector<Decl*> declVec(Decls.begin(), Decls.end());
   //endregion

   //region 获取主文件ID，文件路径
   FileID mainFileId;
   std::string filePath;
   Util::getMainFileIDMainFilePath(SM,mainFileId,filePath);
   //endregion

   //region 若是系统文件 或 tick文件则跳过
   if(Util::isSysSrcFile(filePath)  || Util::isTickSrcFile(filePath)){
     return ;
   }
   //endregion

   //region 打印文件路径 开发用
   FrontendOptions &frontendOptions = CI.getFrontendOpts();
   std::cout << "查看，文件路径:" << filePath << ",mainFileId:" << mainFileId.getHashValue() << ",frontendOptions.ProgramAction:" << frontendOptions.ProgramAction << "，Ctx.TUKind:" << Ctx.TUKind <<  std::endl;
   //endregion
   
   //region 复制源文件 到 /build/srcCopy/, 开关copySrcFile=true.
   // (适合cmake测试编译器，源文件用完即删除，导致此时出问题后拿不到源文件，难以复现问题）
   if(Util::envVarEq("copySrcFile","true")){
     Util::copySrcFile(filePath,"/build/srcCopy/");
   }
   //endregion

   //region 1.若本文件已处理，则直接返回。
   if(BrcAstCnsm::isProcessed(CI,SM,Ctx,brcOk,declVec)){
     return ;
   }
   //endregion

   //region 2. 调用 花括号遍历器 遍历每个声明， 以插入花括号
   unsigned long declCnt = declVec.size();
   for(int i=0; i<declCnt; i++) {
     Decl *D = declVec[i];
     //跳过非MainFile中的声明
     if(!Util::isDeclInMainFile(SM,D)){
       continue;
     }
     //只处理MainFile中的声明
     this->brcVst.TraverseDecl(D);
   }
   //endregion

   //region 3. 插入 已处理 注释标记 到主文件第一个声明前
   //如果 花括号遍历器 确实有进行过至少一次插入花括号 , 才插入 已处理 注释标记
   if( !(brcVst.LBraceLocIdSet.empty()) ){
   bool insertResult;
   //插入的注释语句不要带换行,这样不破坏原行号
   //  必须插入此样式/** */ 才能被再次读出来， 而/* */读不出来
   const std::string brcOkFlagComment = fmt::format("/**{}*/", BrcOkFlagText);
   Decl* firstDeclInMainFile=Util::firstDeclInMainFile(SM,declVec);
   if(firstDeclInMainFile){
     Util::insertCommentBeforeLoc(brcOkFlagComment, firstDeclInMainFile->getBeginLoc(),  brcVst.mRewriter_ptr, insertResult);
   }
   }


   //endregion

   //region 4. 应用修改到源文件
   //如果 花括号遍历器 确实有进行过至少一次插入花括号 , 才应用修改到源文件
   if( !(brcVst.LBraceLocIdSet.empty()) ){
   brcVst.mRewriter_ptr->overwriteChangedFiles();
   DiagnosticsEngine &Diags = CI.getDiagnostics();
   std::cout <<  Util::strDiagnosticsEngineHasErr(Diags) << std::endl;
   }
   //endregion
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
