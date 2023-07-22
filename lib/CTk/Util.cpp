
#include "CTk/Util.h"

#include <clang/Rewrite/Core/Rewriter.h>
#include <clang/Frontend/CompilerInstance.h>
#include "clang/AST/Stmt.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include <clang/AST/ParentMapContext.h>

#include <string>
#include <iostream>


using namespace llvm;
using namespace clang;

void Util::insertIncludeToFileStartByLoc(StringRef includeStmtText,SourceLocation Loc, SourceManager &SM, Rewriter& rewriter){
  FileID fileId = SM.getFileID(Loc);

  insertIncludeToFileStart(includeStmtText,fileId,SM,rewriter);
}

void Util::insertIncludeToFileStart(StringRef includeStmtText,FileID fileId, SourceManager &SM, Rewriter& rewriter)   {
//  SourceManager &SM = Context.getSourceManager();
//  FileID MainFileID = SM.getMainFileID();

//  FileID fileId = SM.getFileID(Loc);
  SourceLocation startLoc = SM.getLocForStartOfFile(fileId);

  const RewriteBuffer *RewriteBuf = rewriter.getRewriteBufferFor(fileId);
  if (!RewriteBuf){
    return;
  }


  rewriter.InsertText(startLoc, includeStmtText, true, true);
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
bool Util::getSourceFilePathOfStmt(const Stmt *S, const SourceManager &SM,StringRef& fn) {
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

void Util::printExpr(CompilerInstance &CI, std::string tag, std::string title, clang::Expr *expr,bool printSourceText) {
  SourceManager & SM=CI.getSourceManager();
  const char *kindName = expr->getStmtClassName();
  Stmt::StmtClass kind = expr->getStmtClass();
  FileID fileId = SM.getFileID(expr->getBeginLoc());
  SourceRange sourceRange=expr->getSourceRange();
  int zz=static_cast<int>(kind);
  printSourceRange(CI,tag,title,fileId,sourceRange,kindName,static_cast<int>(kind),printSourceText);

}
void  Util::printDecl(CompilerInstance& CI, std::string tag,std::string title,clang::Decl* decl,bool printSourceText){
  SourceManager & SM=CI.getSourceManager();
  const char *kindName = decl->getDeclKindName();
  Decl::Kind kind = decl->getKind();
  FileID fileId = SM.getFileID(decl->getBeginLoc());
  SourceRange sourceRange=decl->getSourceRange();
  printSourceRange(CI,tag,title,fileId,sourceRange,kindName,kind,printSourceText);

}
void  Util::printSourceRange(CompilerInstance& CI, std::string tag,std::string title,FileID fileId,const SourceRange &sourceRange,const char *kindName,Decl::Kind kind,bool printSourceText){
  SourceManager & SM=CI.getSourceManager();
  FileID mainFileId = SM.getMainFileID();
//  FileID fileId = SM.getFileID(sourceRange.getBegin());

  const std::tuple<std::string, std::string> & frst = get_FileAndRange_SourceText(sourceRange,CI);
  std::string fileAndRange=std::get<0>(frst);
  std::string sourceText=std::get<1>(frst);

  std::cout << tag << "," << title << ",文件路径、坐标:"<< fileAndRange <<   ",kindName:" << kindName<<  ",kind:" << kind ;
  if(printSourceText){
    std::cout <<   ",源码:" << sourceText ;
  }
  std::cout <<  ",mainFileId:" << mainFileId.getHashValue() << ",fileId:" << fileId.getHashValue() << std::endl;

}