
#include "CTk/Util.h"

#include <clang/Rewrite/Core/Rewriter.h>
#include <clang/Frontend/CompilerInstance.h>
#include "clang/AST/Stmt.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include <clang/AST/ParentMapContext.h>

#include <string>
#include <iostream>
#include <fstream>


using namespace llvm;
using namespace clang;

bool Util::isMacroLocation(SourceLocation loc, SourceManager &SM) {
  bool isMacroArgExpansion = SM.isMacroArgExpansion(loc);
  bool isMacroBodyExpansion= SM.isMacroBodyExpansion(loc);
  bool isMacroLoc=isMacroArgExpansion || isMacroBodyExpansion;
  return isMacroLoc;
}
bool Util::envVarEq(std::string varName, std::string varValueExpect){
  if(varName.empty()){
    return false;
  }
  const char* varValueReal=std::getenv(varName.c_str());
  if(varValueReal == NULL){
    return false;
  }
  bool eq= (varValueExpect == varValueReal);
  return eq;
}

void Util::saveEditBuffer(const std::shared_ptr<Rewriter> rewriter_ptr, FileID mainFileId, std::string filePath) {
  RewriteBuffer &editBuffer = rewriter_ptr->getEditBuffer(mainFileId);
  Util::saveRewriteBuffer0(&editBuffer,filePath,"saveEditBuffer:");
}

std::string Util::rewriteBufferToString(const RewriteBuffer &buffer) {
  return std::string(buffer.begin(), buffer.end());
}
void Util::saveRewriteBuffer(const std::shared_ptr<Rewriter> rewriter_ptr, FileID mainFileId, std::string filePath) {
  const RewriteBuffer *pRewriteBuffer = rewriter_ptr->getRewriteBufferFor(mainFileId);
  Util::saveRewriteBuffer0(pRewriteBuffer,filePath,"saveRewriteBuffer:");
}

void Util::saveRewriteBuffer0(const RewriteBuffer *pRewriteBuffer,std::string filePath,std::string title){
//  const RewriteBuffer *pRewriteBuffer = rewriter_ptr->getRewriteBufferFor(mainFileId);
  std::string cppText = rewriteBufferToString(*pRewriteBuffer);

  std::ofstream fWriter;
  fWriter.open(filePath);
  fWriter << cppText ;
  fWriter.close();

  std::cout << title << filePath <<std::endl;
}
bool Util::isLastCompoundStmt(CompoundStmt *stmt, ASTContext &context) {
  auto parents = context.getParents(*stmt);

  // 遍历父节点列表
  for (auto it = parents.begin(); it != parents.end(); ++it) {
    if (const FunctionDecl *func = it->get<FunctionDecl>()) {
      // 检查CompoundStmt是否为最后一个块
      Stmt *body = func->getBody();
      if (body && body == stmt) {
        return true;
      }
    } else if (const LambdaExpr *lambda = it->get<LambdaExpr>()) {
      // 检查CompoundStmt是否为lambda表达式的最后一个块
      Stmt *body = lambda->getBody();
      if(body){
        if (CompoundStmt *lambdaBody = dyn_cast<CompoundStmt>(body)) {
          Stmt *lastStmt = lambdaBody->body_back();
          if (lastStmt && lastStmt == stmt) {
            return true;
          }
        }
      }

    }
  }

  return false;
}
FunctionDecl *Util::getContainingFunction(CompoundStmt *stmt, ASTContext &context) {
  auto parents = context.getParents(*stmt);

  // 遍历父节点列表
  for (auto itJ = parents.begin(); itJ != parents.end(); ++itJ) {
    if (const LambdaExpr *lambdaJ = itJ->get<LambdaExpr>()) {
      // 返回包裹CompoundStmt的lambda
      CXXMethodDecl *methodJ = lambdaJ->getCallOperator();
      if (methodJ) {
        return methodJ;
      }
    } else if (const FunctionDecl *funcJ = itJ->get<FunctionDecl>()) {
      // 返回最近的FunctionDecl
      return const_cast<FunctionDecl*>(funcJ);
    }
  }

  return nullptr;
}



Stmt* Util::endStmtOfFunc(FunctionDecl *funcDecl) {
  Stmt *funcBody = funcDecl->getBody();
  if (funcBody && isa<CompoundStmt>(*funcBody)) {
    CompoundStmt *compoundStmt = dyn_cast<CompoundStmt>(funcBody);
    if (compoundStmt &&  (!compoundStmt->body_empty() ) ) {
      Stmt *lastStmt = compoundStmt->body_back();
      return lastStmt;
    }
  }
  return NULL;
}
bool Util::isReturnStmtClass(Stmt *stmt ){
  bool stmtIsReturn=false;
  if(stmt){
    Stmt::StmtClass endStmtClass = stmt->getStmtClass();
    if(Stmt::ReturnStmtClass==endStmtClass){
      stmtIsReturn=true;
    }
  }
  return stmtIsReturn;
}

std::vector<bool>  Util::subStmtIsFallThroughVec(const Stmt::child_range &subStmtLs ,Stmt* &negativeSecond,SourceManager& SM, LangOptions& langOptions) {
  std::vector<clang::Stmt*> subStmtVec(subStmtLs.begin(), subStmtLs.end());
  unsigned long subStmtCnt = subStmtVec.size();
  const std::vector<std::string> &textVec = Util::stmtLs2TextLs(subStmtVec, SM, langOptions);
  if(subStmtCnt>=2){
    //倒数第二条语句
    negativeSecond=subStmtVec[subStmtCnt-2];
  }

  //subStmtVec中的stmtJ是否为'[[gnu::fallthrough]];'
  std::vector<bool> subStmtIsFallThroughVec(subStmtCnt,false);

  for (std::size_t j = 0; j < subStmtCnt; ++j) {
    clang::Stmt* stmtJ = subStmtVec[j];
    subStmtIsFallThroughVec[j]=Util::hasAttrKind(stmtJ, attr::Kind::FallThrough);
      //如果本行语句是'[[gnu::fallthrough]];'  , 那么下一行前不要插入时钟语句, 否则语法错误.
  }
  return subStmtIsFallThroughVec;
}

bool Util::hasAttrKind(Stmt *stmt, attr::Kind attrKind){
  if(!stmt){
    return false;
  }
//  clang::AttributedStmt* attributedStmt = clang::dyn_cast<clang::AttributedStmt>(stmt);

  clang::AttributedStmt* attributedStmt = clang::dyn_cast_or_null<clang::AttributedStmt>(stmt);

//  AttributedStmt* attributedStmt= static_cast<AttributedStmt*> (stmt);//不能这样转，static_cast会把一个不是AttributedStmt的东西强硬转为AttributedStmt, 比如把一段不可写的代码区域转为Attr对象, 显然导致Segmentation fault而崩溃退出.

  if(attributedStmt){
    const ArrayRef<const Attr *> &attrS = attributedStmt->getAttrs();
    std::vector<const Attr *> attrVec(attrS.begin(), attrS.end());//方便调试看数组内容
    for(const Attr * attrJ:attrVec){
      if(!attrJ){
        continue;
      }
      attr::Kind attrJKind = attrJ->getKind();
      const std::string &normalizedFullName = attrJ->getNormalizedFullName();
//      std::cout << "AttributedStmt:" << attrJKind << "," << normalizedFullName << std::endl;
//    AttributedStmt:24,gnu::fallthrough
      if(attrKind==attrJKind){
        return true;
      }
    }//for结束

  }//if结束

  return false;
}
void Util::extractLineAndColumn(const clang::SourceManager& SM, const clang::SourceLocation& sourceLocation, int& line, int& column) {
  clang::PresumedLoc presumedLoc = SM.getPresumedLoc(sourceLocation);
  line = presumedLoc.getLine();
  column = presumedLoc.getColumn();
}

bool Util::parentIsCompound(ASTContext* astContext, const Stmt* currentStmt) {
  bool parentKindIsCompound= Util::parentKindIsSame(astContext, currentStmt, ASTNodeKind::getFromNodeKind<CompoundStmt>());
  bool parentClassIsCompound= Util::parentClassEqual(astContext, currentStmt, Stmt::CompoundStmtClass);
  bool parentIsCompound=parentKindIsCompound||parentClassIsCompound;
  return parentIsCompound;
}

bool Util::parentClassEqual(ASTContext* astContext, const Stmt* stmt, Stmt::StmtClass targetClass) {
  auto parents = astContext->getParents(*stmt);

  for (const auto& parent : parents) {
    auto stmtParent = parent.get<Stmt>();
    if (stmtParent && stmtParent->getStmtClass() == targetClass) {
      return true;
    }
  }

  return false;
}


bool Util::parentKindIsSame(ASTContext *Ctx, const Stmt* stmt, const ASTNodeKind& kind){
  if(!Ctx || !stmt){
    return false;
  }
  DynTypedNodeList parentS=Ctx->getParents(*stmt);
  for (const auto& parent : parentS) {
    if (   kind.isSame(parent.getNodeKind())  ) {
      return true;
    }
  }

  return false;
}


/**取得声明语句中声明的变量个数
 * 在声明语句 中 声明的变量个数
 * 比如 :
 * 输入 "int i;",  返回1
 * 输入 "float sum,x=0,y;", 返回3
 * 输入 非声明语句,  返回0
 * 输入 非变量声明语句,  返回0
 * @param stmt
 * @return
 */
int Util::varCntInVarDecl(DeclStmt* declStmt) {
//  DeclStmt *declStmt = static_cast<DeclStmt *>(stmt);
  if(declStmt==NULL){
    return 0;
  }
//  if(declStmt){
    Decl *decl0 = *(declStmt->decl_begin());
    if(decl0 && decl0->getKind()==Decl::Kind::Var){
      //如果当前语句是声明语句, 且第一个子声明是变量声明语句,则栈变量分配个数填写1
      //  有可能是这种样子: int n,m,u,v=0;  应该取 declStmt->decls() 的size
      const DeclStmt::decl_range &declRange = declStmt->decls();
      // 取 declStmt->decls() 的size
      long declCnt = std::distance(declRange.begin(), declRange.end());
      return declCnt;
    }
    return 0;
//  }
}
void Util::insertIncludeToFileStartByLoc(StringRef includeStmtText,SourceLocation Loc, SourceManager &SM, const std::shared_ptr<Rewriter> mRewriter_ptr){
  FileID fileId = SM.getFileID(Loc);

  insertIncludeToFileStart(includeStmtText,fileId,SM,mRewriter_ptr);
}

void Util::insertIncludeToFileStart(StringRef includeStmtText,FileID fileId, SourceManager &SM, const std::shared_ptr<Rewriter> mRewriter_ptr)   {
//  SourceManager &SM = Context.getSourceManager();
//  FileID MainFileID = SM.getMainFileID();

//  FileID fileId = SM.getFileID(Loc);
  SourceLocation startLoc = SM.getLocForStartOfFile(fileId);

  const RewriteBuffer *RewriteBuf = mRewriter_ptr->getRewriteBufferFor(fileId);
  if (!RewriteBuf){
    return;
  }


  bool insertResult=mRewriter_ptr->InsertText(startLoc, includeStmtText, true, true);
  if(!insertResult){
    std::cerr<<"05插入返回false"<<std::endl;
  }
}

FunctionDecl* Util::findFuncDecByName(ASTContext *Ctx,std::string functionName){
//    std::string functionName = "calc";

  TranslationUnitDecl* translationUnitDecl=Ctx->getTranslationUnitDecl();
  for(auto decl:translationUnitDecl->decls()){
    if(FunctionDecl* funcDecl = dyn_cast<FunctionDecl>(decl)){
      if(funcDecl->getNameAsString()==functionName){
        return funcDecl;
      }
    }
  }
  return NULL;
}

std::vector<std::string> Util::stmtLs2TextLs(std::vector<Stmt*> stmtVec, SourceManager & SM, const LangOptions & langOptions){
  std::vector<std::string> textVec;

  std::transform(stmtVec.begin(), stmtVec.end(), std::back_inserter(textVec), [&SM,&langOptions](Stmt* stmt) {
      return getSourceTextBySourceRange(stmt->getSourceRange(),SM,langOptions); // 这里可以根据需要进行转换操作
  });

  return textVec;
}
/**
 * 获取 给定 位置范围 的源码文本
 * @param sourceRange
 * @param sourceManager
 * @param langOptions
 * @return
 */
std::string Util::getSourceTextBySourceRange(SourceRange sourceRange, SourceManager & sourceManager, const LangOptions & langOptions){
  //ref:  https://stackoverflow.com/questions/40596195/pretty-print-statement-to-string-in-clang/40599057#40599057
//  SourceRange sourceRange=S->getSourceRange();
  CharSourceRange charSourceRange=CharSourceRange::getCharRange(sourceRange);
  llvm::StringRef strRefSourceText=Lexer::getSourceText(charSourceRange, sourceManager, langOptions);

  std::string strSourceText=strRefSourceText.str();
  return strSourceText;
}

/**
 * 获取语句所属源文件路径
 */
void Util::getSourceFilePathOfStmt(const Stmt *S, const SourceManager &SM,StringRef& fn) {
  SourceLocation Loc = S->getBeginLoc();
  Util::getSourceFilePathAtLoc(Loc,SM,fn);
}

/**
 * 获取位置所属源文件路径
 * 获取语句所属源文件路径
 * code by chatgpt on : https://chat.chatgptdemo.net/
 * @param S
 * @param SM
 * @param fn
 * @return
 */
bool Util::getSourceFilePathAtLoc(SourceLocation Loc, const SourceManager &SM,StringRef& fn) {
//  SourceLocation Loc = S->getBeginLoc();
  if (Loc.isValid()) {
    FileID File = SM.getFileID(Loc);
    const FileEntry *FE = SM.getFileEntryForID(File);
    if (FE) {
      fn=FE->getName();
//      llvm::outs() << "Source File Path: " << FE->getName() << "\n";
      return true;
    }
  }
  return false;
}

/**开发用工具 get_FileAndRange_SourceText ： 获得SourceRange的 文件路径、文件中的坐标、源码文本
 *
 * @param sourceRange
 * @param CI
 * @return
 */

std::tuple<std::string,std::string>  Util::get_FileAndRange_SourceText(const SourceRange &sourceRange,CompilerInstance& CI){
  //{开发用
  SourceManager &SM = CI.getSourceManager();
  LangOptions &langOpts = CI.getLangOpts();
//      const SourceRange &sourceRange = Decl->getSourceRange();
  std::string fileAndRange = sourceRange.printToString(SM);
  std::string sourceText = Util::getSourceTextBySourceRange(sourceRange, SM, langOpts);
  return std::tuple<std::string,std::string>(fileAndRange,sourceText);
  //}
}

void Util::printStmt(ASTContext &Ctx, CompilerInstance &CI, std::string tag, std::string title, clang::Stmt *stmt,
                     bool printSourceText) {
  int64_t stmtID = stmt->getID(Ctx);
  SourceManager & SM=CI.getSourceManager();
  const char *stmtClassName = stmt->getStmtClassName();
  Stmt::StmtClass stmtClass = stmt->getStmtClass();
  FileID fileId = SM.getFileID(stmt->getBeginLoc());
  SourceRange sourceRange=stmt->getSourceRange();

  printSourceRange(stmtID,
                   CI,
                   tag, title,
                   fileId, sourceRange,
                   "getStmtClassName", stmtClassName,
                   "getStmtClass", stmtClass,
                   NULL,EMPTY_ENUM_VAL,
                   NULL,EMPTY_ENUM_VAL,
                   printSourceText);

}
void Util::printExpr(ASTContext &Ctx, CompilerInstance &CI, std::string tag, std::string title, clang::Expr *expr,
                     bool printSourceText) {
  int64_t exprID = expr->getID(Ctx);
  SourceManager & SM=CI.getSourceManager();
  const char *stmtClassName = expr->getStmtClassName();
  Stmt::StmtClass stmtClass = expr->getStmtClass();
  ExprValueKind valueKind = expr->getValueKind();
  ExprObjectKind objectKind = expr->getObjectKind();
  FileID fileId = SM.getFileID(expr->getBeginLoc());
  SourceRange sourceRange=expr->getSourceRange();

  printSourceRange(exprID,
                   CI,
                   tag, title,
                   fileId, sourceRange,
                   "getStmtClassName", stmtClassName,
                   "getStmtClass", stmtClass,
                   "getValueKind", valueKind,
                   "getObjectKind", objectKind,
                   printSourceText);

}
void Util::printDecl(ASTContext &Ctx, CompilerInstance &CI, std::string tag, std::string title, clang::Decl *decl,
                     bool printSourceText) {
  int64_t declID = decl->getID();
  unsigned int declGlobalID = decl->getGlobalID();
  SourceManager & SM=CI.getSourceManager();
  const char *kindName = decl->getDeclKindName();
  Decl::Kind kind = decl->getKind();
  FileID fileId = SM.getFileID(decl->getBeginLoc());
  SourceRange sourceRange=decl->getSourceRange();
  printSourceRange(declID,
                   CI,
                   tag,title,
                   fileId,sourceRange,
                   "getDeclKindName",kindName,
                   "getKind",kind,
                   NULL,EMPTY_ENUM_VAL,
                   NULL,EMPTY_ENUM_VAL,
                   printSourceText);

}
void  Util::printSourceRange(int64_t nodeID,
        CompilerInstance& CI,
        std::string tag, std::string title,
        FileID fileId, const SourceRange &sourceRange,
        const char *topCategoryFieldName, const char *topCategoryName,
        const char *topCategoryEnumFieldName, int topCategoryEnum,
        const char *category1FieldName, int category1Enum,
        const char *category2FieldName,int category2Enum,
        bool printSourceText){
  SourceManager & SM=CI.getSourceManager();
  FileID mainFileId = SM.getMainFileID();
//  FileID fileId = SM.getFileID(sourceRange.getBegin());

  const std::tuple<std::string, std::string> & frst = get_FileAndRange_SourceText(sourceRange,CI);
  std::string fileAndRange=std::get<0>(frst);
  std::string sourceText=std::get<1>(frst);


  std::cout
    << tag << ","
    << title
    << ",nodeID:" << nodeID
    << ",文件路径、坐标:" << fileAndRange
    << "," << topCategoryFieldName << ":" << topCategoryName
    << "," << topCategoryEnumFieldName << ":" << topCategoryEnum
    <<  ",mainFileId:" << mainFileId.getHashValue()
    << ",fileId:" << fileId.getHashValue()
    ;
  if(category1Enum!=EMPTY_ENUM_VAL){
  std::cout
    << "," << category1FieldName << ":" << category1Enum
    ;
  }
  if(category2Enum!=EMPTY_ENUM_VAL){
    std::cout
      << "," << category2FieldName << ":" << category2Enum
    ;
  }
  if(printSourceText){
    std::cout <<   ",源码:" << sourceText ;
  }

  std::cout  << std::endl;

}