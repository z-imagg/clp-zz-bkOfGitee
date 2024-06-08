#include "Var/VarAstCnsm.h"
#include "Var/CollectIncMacro_PPCb.h"
#include "Var/Constant.h"

#include "base/MyAssert.h"
#include "base/UtilInsertInclude.h"
#include "base/UtilMainFile.h"
#include "base/UtilIsSysSrcFileOrMe.h"
#include "base/UtilFile.h"
#include "base/UtilEnvVar.h"
#include "base/UtilDiagnostics.h"
#include <llvm/Support/Casting.h>


bool VarAstCnsm::mainFileProcessed=false;


 void VarAstCnsm::HandleTranslationUnit(ASTContext &Ctx) {
     ///region 在此编译进程内, 跳过已处理的mainFile, 避免重复处理
     //被上层多次调用 本方法HandleTranslationUnit，后续的调用不再进入实际处理
     if(mainFileProcessed){
         return;
     }
     //endregion

     ///region 打印各重要对象地址
   std::cout << fmt::format("HandleTranslationUnit打印各重要对象地址: CI:{:x},this->Ctx:{:x},Ctx:{:x},SM:{:x},mRewriter_ptr:{:x}",
reinterpret_cast<uintptr_t> (&CI ),
reinterpret_cast<uintptr_t> (&(this->Ctx) ),
reinterpret_cast<uintptr_t> (&Ctx ),
reinterpret_cast<uintptr_t> (&SM ),
reinterpret_cast<uintptr_t> ( (fnVst.mRewriter_ptr.get()) ) ) << std::endl;
//  ASTConsumer::HandleTranslationUnit(Ctx);
    //endregion

   TranslationUnitDecl *translationUnitDecl = Ctx.getTranslationUnitDecl();


   //region 顶层翻译单元 的 声明们 既有 主文件的 又有 非主文件的，故 在顶层 不合适 做 跳过 非主文件
   //  translationUnitDecl中同时包含 非MainFile中的Decl、MainFile中的Decl
   //    因此不能用translationUnitDecl的位置 判断当前是否在MainFile中
//  if(!UtilMainFile::isDeclInMainFile(SM,translationUnitDecl)){
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
   UtilMainFile::getMainFileIDMainFilePath(SM,mainFileId,filePath);
   //endregion

   ///region 若是系统文件 或 tick文件则跳过
   if(UtilIsSysSrcFileOrMe::isSysSrcFile(filePath)  || UtilIsSysSrcFileOrMe::isRuntimeSrcFile(filePath,"runtime_cpp__vars_fn")){
     return ;
   }
   //endregion

   ///region 打印文件路径 开发用
   FrontendOptions &frontendOptions = CI.getFrontendOpts();
   std::cout << "查看，文件路径:" << filePath << ",mainFileId:" << mainFileId.getHashValue() << ",frontendOptions.ProgramAction:" << frontendOptions.ProgramAction << "，Ctx.TUKind:" << Ctx.TUKind <<  std::endl;
   //endregion
   
   ///region 复制源文件 到 /build/srcCopy/, 开关copySrcFile=true.
   // (适合cmake测试编译器，源文件用完即删除，导致此时出问题后拿不到源文件，难以复现问题）
   if(UtilEnvVar::envVarEq("copySrcFile","true")){
     UtilFile::copySrcFile(filePath,"/build/srcCopy/");
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
   //endregion

   ///region 2. 调用 花括号遍历器 遍历每个声明， 以插入花括号
   unsigned long declCnt = declVec.size();
   for(int i=0; i<declCnt; i++) {
     Decl *D = declVec[i];
     //跳过非MainFile中的声明
     if(!UtilMainFile::isDeclInMainFile(SM,D)){
       continue;
     }

     //只处理MainFile中的声明

     Decl::Kind declKind = D->getKind();
     const char *declKindName = D->getDeclKindName();

     //访问头文件中某类的方法体列表
     if (D && llvm::isa<clang::CXXRecordDecl>(*D)) {
       if(clang::CXXRecordDecl *cxxRecordDecl = dyn_cast<clang::CXXRecordDecl>(D)){
         //class CxxUser{ cxx方法1(){ 方法体 }  ...   cxx方法k(){ 方法体 }  }
//         cxxRecordDecl->methods();
         CXXDestructorDecl *deconstor = cxxRecordDecl->getDestructor();
         std::vector<CXXMethodDecl*> cxxMethodDeclVec(cxxRecordDecl->method_begin(), cxxRecordDecl->method_end());
         std::for_each(cxxRecordDecl->method_begin(), cxxRecordDecl->method_end(), [this](  CXXMethodDecl* cxxMethodDecl){
           // cxx方法k
           // A1、B1、C1需要保持顺序一致么？
           bool focusFunc=this->fnVst.TraverseCXXMethodDecl(cxxMethodDecl);//A1
           if(focusFunc){
             this->retVst.TraverseCXXMethodDecl(cxxMethodDecl);//B1
             this->varDeclVst.TraverseCXXMethodDecl(cxxMethodDecl);//C1
           }
         });

       }
     }else
     //访问实现文件中某方法体
     if (D && llvm::isa<clang::FunctionDecl>(*D)) {
       if(clang::FunctionDecl *funDecl = dyn_cast<clang::FunctionDecl>(D)){
         // CUser::cxx方法j(){方法体}  , 普通方法i(){方法体}
         // A1、B1、C1需要保持顺序一致么？
         bool focusFunc=this->fnVst.TraverseDecl(funDecl);//A1
         if(focusFunc){
           this->retVst.TraverseDecl(funDecl);//B1
           this->varDeclVst.TraverseDecl(funDecl);//C1
         }
       }
     }else{
//       const std::string &msg = fmt::format("跳过不关心的Decl，declKind={},declKindName={}\n\n", int(declKind), declKindName);
//       std::cout<<msg;
     }

     //clang::Decl::Function //clang::Decl::CXXRecord
     // TODO 若函数声明D 的身体只占据一行（  即 '{'和'}'在同一行， 则 忽略 即 不处理， 情况举例 /shl_app/qemu/util/module.c ）
   }
   //endregion

   ///region 3. 插入 已处理 注释标记 到主文件第一个声明前
     bool insertResult;
     UtilInsertInclude::insertIncludeToFileStart(c.PrgMsgStmt_funcIdAsmIns, mainFileId, SM, fnVst.mRewriter_ptr, insertResult);//此时  insertVst.mRewriter.getRewriteBufferFor(mainFileId)  != NULL， 可以做插入
     std::string msg=fmt::format("插入'#pragma 消息'到文件{},对mainFileId:{},结果:{}\n",filePath,mainFileId.getHashValue(),insertResult);
     std::cout<< msg ;


   //endregion

   //断言两个Vst中的rewriter是同一个对象
   MyAssert(varDeclVst.mRewriter_ptr==fnVst.mRewriter_ptr,"[AssertErr]NotFit:varDeclVst.mRewriter_ptr==fnVst.mRewriter_ptr")
   MyAssert(fnVst.mRewriter_ptr==retVst.mRewriter_ptr,"[AssertErr]NotFit:fnVst.mRewriter_ptr==retVst.mRewriter_ptr")

   ///region 4. 应用修改到源文件
   //如果 花括号遍历器 确实有进行过至少一次插入花括号 , 才应用修改到源文件
     // 指向同一个对象的多个std::shared_ptr<Rewriter>确实是同一个对象（这些shared_ptr<Rewriter>.overwriteChangedFiles调用等同于单个）
     if( (!(fnVst.fnBdLBrcLocIdSet.empty())) || (!(retVst.retBgnLocIdSet.empty())) || (!(varDeclVst.VarDeclLocIdSet.empty()))  ){
         fnVst.mRewriter_ptr->overwriteChangedFiles();//A1写
     }
     DiagnosticsEngine &Diags = CI.getDiagnostics();
     std::cout <<  UtilDiagnostics::strDiagnosticsEngineHasErr(Diags) << std::endl;
   //endregion

   ///region 在此编译进程内, 标记本mainFile已处理, 避免重复处理
    //可以发现, 本方法 两次被调用 ， 对象地址this 即对象CTkAstCnsm的地址，两次是不同的。 原因在Act中 是 每次都是 新创建 CTkAstCnsm。
    mainFileProcessed=true;
   //endregion
 }

