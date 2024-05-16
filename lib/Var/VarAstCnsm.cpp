#include "Var/VarAstCnsm.h"
#include "Var/CollectIncMacro_PPCb.h"
#include "Var/Constant.h"



bool VarAstCnsm::mainFileProcessed=false;


std::string VarAstCnsm::VarOkFlagText="__VarOkFlagText";

 void VarAstCnsm::HandleTranslationUnit(ASTContext &Ctx) {
     ///region 在此编译进程内, 跳过已处理的mainFile, 避免重复处理
     //被上层多次调用 本方法HandleTranslationUnit，后续的调用不再进入实际处理
     if(mainFileProcessed){
         return;
     }
     //endregion

     ///region 打印各重要对象地址
   std::cout<< fmt::format("HandleTranslationUnit打印各重要对象地址: CI:{:x},this->Ctx:{:x},Ctx:{:x},SM:{:x},mRewriter_ptr:{:x}",
reinterpret_cast<uintptr_t> (&CI ),
reinterpret_cast<uintptr_t> (&(this->Ctx) ),
reinterpret_cast<uintptr_t> (&Ctx ),
reinterpret_cast<uintptr_t> (&SM ),
reinterpret_cast<uintptr_t> ( (varVst.mRewriter_ptr.get()) ) ) <<std::endl;
//  ASTConsumer::HandleTranslationUnit(Ctx);
    //endregion

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

   ///region 获取主文件ID，文件路径
   FileID mainFileId;
   std::string filePath;
   Util::getMainFileIDMainFilePath(SM,mainFileId,filePath);
   //endregion

   ///region 若是系统文件 或 tick文件则跳过
   if(Util::isSysSrcFile(filePath)  || Util::isRuntimeSrcFile(filePath,"runtime_cpp__vars_fn")){
     return ;
   }
   //endregion

   ///region 打印文件路径 开发用
   FrontendOptions &frontendOptions = CI.getFrontendOpts();
   std::cout << "查看，文件路径:" << filePath << ",mainFileId:" << mainFileId.getHashValue() << ",frontendOptions.ProgramAction:" << frontendOptions.ProgramAction << "，Ctx.TUKind:" << Ctx.TUKind <<  std::endl;
   //endregion
   
   ///region 复制源文件 到 /build/srcCopy/, 开关copySrcFile=true.
   // (适合cmake测试编译器，源文件用完即删除，导致此时出问题后拿不到源文件，难以复现问题）
   if(Util::envVarEq("copySrcFile","true")){
     Util::copySrcFile(filePath,"/build/srcCopy/");
   }
   //endregion

     Constant c;

   ///region 1.若本文件已处理，则直接返回。
 {

     bool hasPragmaMsg = CollectIncMacro_PPCb::pragma_message_set.find(c.NameSpace_funcIdAsmIns) != CollectIncMacro_PPCb::pragma_message_set.end();
     if(hasPragmaMsg){
         //若已经有#include "funcIdBase.h"，则标记为已处理，且直接返回，不做任何处理。
         std::cout << fmt::format("跳过，因为此文件已经被处理, 文件路径:{} 已经包含#pragma消息 {}\n",filePath,c.PrgMsgStmt_funcIdAsmIns) ;
         return;
     }
 }
   if(VarAstCnsm::isProcessed(CI,SM,Ctx,varOk,declVec)){
     return ;
   }
   //endregion

   ///region 2. 调用 花括号遍历器 遍历每个声明， 以插入花括号
   unsigned long declCnt = declVec.size();
   for(int i=0; i<declCnt; i++) {
     Decl *D = declVec[i];
     //跳过非MainFile中的声明
     if(!Util::isDeclInMainFile(SM,D)){
       continue;
     }
     //只处理MainFile中的声明
     this->varVst.TraverseDecl(D);
   }
   //endregion

   ///region 3. 插入 已处理 注释标记 到主文件第一个声明前
     bool insertResult;
     Util::insertIncludeToFileStart(c.PrgMsgStmt_funcIdAsmIns, mainFileId, SM, varVst.mRewriter_ptr,insertResult);//此时  insertVst.mRewriter.getRewriteBufferFor(mainFileId)  != NULL， 可以做插入
     std::string msg=fmt::format("插入'#pragma 消息'到文件{},对mainFileId:{},结果:{}\n",filePath,mainFileId.getHashValue(),insertResult);
     std::cout<< msg ;


   //endregion

   ///region 4. 应用修改到源文件
   //如果 花括号遍历器 确实有进行过至少一次插入花括号 , 才应用修改到源文件
   if( !(varVst.VarDeclLocIdSet.empty()) ){
   varVst.mRewriter_ptr->overwriteChangedFiles();
   DiagnosticsEngine &Diags = CI.getDiagnostics();
   std::cout <<  Util::strDiagnosticsEngineHasErr(Diags) << std::endl;
   }
   //endregion

   ///region 在此编译进程内, 标记本mainFile已处理, 避免重复处理
    //可以发现, 本方法 两次被调用 ， 对象地址this 即对象CTkAstCnsm的地址，两次是不同的。 原因在Act中 是 每次都是 新创建 CTkAstCnsm。
    mainFileProcessed=true;
   //endregion
 }

//region 判断是否已经处理过了
bool VarAstCnsm::isProcessed(CompilerInstance& CI,SourceManager&SM, ASTContext& Ctx,  bool& _varOk, std::vector<Decl*> declVec){
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
     VarAstCnsm::__visitRawComment(CI,SM,  rc, _varOk);
     if(_varOk){
       std::cout<<"已有标记(已插入花括号),不再处理"<<std::endl;
       return true;
     }
   }
   return false;
}
void VarAstCnsm::__visitRawComment(CompilerInstance& CI,SourceManager&SM, const RawComment *C, bool & _varOk) {
  if(!C){
    return;
  }
  const SourceRange &sourceRange = C->getSourceRange();
//  Util::printSourceRangeSimple(CI,"查看RawComment","",caseKSrcRange, true);

  if(_varOk){
    return;
  }

  LangOptions &langOpts = CI.getLangOpts();
  std::string sourceText = Util::getSourceTextBySourceRange(sourceRange, SM, langOpts);
//      varOk= (sourceText==VarOkFlagText);//由于取出来的可能是多个块注释，导致不能用相等判断，只能用下面的包含判断
  std::string::size_type index = sourceText.find(VarOkFlagText);
  _varOk= (index != std::string::npos);

}

//endregion
