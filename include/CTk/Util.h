//
// Created by zz on 2023/7/22.
//

#ifndef CLANG_TUTOR_UTIL_H
#define CLANG_TUTOR_UTIL_H


#include <string>

#include <clang/Rewrite/Core/Rewriter.h>
#include <set>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/AST/ParentMapContext.h>
#include <sstream>
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/Stmt.h"
#include "clang/Basic/SourceManager.h"


#include <sstream>
// 放弃 c++11 string_format 宏实现、或std::sprintf实现


using namespace llvm;
using namespace clang;

class Util {
public:
    /**
     *
     * @param funcBody
     * @param funcBodyLBraceLoc
     * @return 是否组合语句,即是否CompoundStmt
     */
    static bool funcBodyLBracLoc(Stmt *funcBody,SourceLocation& funcBodyLBraceLoc);
    /**
     * default函数体举例:
     * 'void func1( ) = default;'
     * 'void func2( ) {}'
     * @param funcDecl
     * @return
     */
    static bool funcIsDefault(FunctionDecl *funcDecl);
    /**
     * default构造函数体举例:
     * 'User::User( ) = default;'
     * 'User::User( ) {}'
     * @param cxxCnstrDecl
     * @return
     */
    static bool cxxConstructorIsDefault(CXXConstructorDecl *cxxCnstrDecl);

    static void emptyStrIfNullStr(const char* &cstr);
    /** c++11 手工实现 string_format
     * 模板，必须要放在头文件中.
     * @tparam Args
     * @param format
     * @param args
     * @return
     */
    


    static std::string pointerToString(void* ptr);
    /**给定源文件路径是否系统源文件
 * 系统源文件路径举例：
/usr/lib/gcc/x86_64-linux-gnu/11/../../../../include/c++/11/bits/cpp_type_traits.h
/usr/lib/gcc/x86_64-linux-gnu/11/../../../../include/c++/11/ext/type_traits.h
/usr/include/x86_64-linux-gnu/bits/iscanonical.h

/app/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/lib/clang/15.0.0/include/uintrintrin.h
 * @param fn
 * @return
 */
    static bool isSysSrcFile(StringRef fn);
    static bool isTickSrcFile(StringRef fn) ;
    static void copySrcFile(std::string srcFilePath,std::string destRootDir="/tmp/");
    static bool LocFileIDEqMainFileID(SourceManager& SM, SourceLocation Loc);
    static bool isMacroLocation(SourceLocation loc, SourceManager &SM);
    static void saveEditBuffer(const std::shared_ptr<Rewriter> rewriter_ptr, FileID mainFileId, std::string filePath);
    static std::string rewriteBufferToString(const RewriteBuffer &buffer);
    static void
    saveRewriteBuffer(const std::shared_ptr<Rewriter> rewriter_ptr, FileID mainFileId, std::string filePath);
    static bool envVarEq(std::string varName, std::string varValueExpect);

    static void saveRewriteBuffer0(const RewriteBuffer *pRewriteBuffer, std::string filePath, std::string title);
    static bool isLastCompoundStmt(CompoundStmt *stmt, ASTContext &context);
    static FunctionDecl *getContainingFunction(CompoundStmt *stmt, ASTContext &context);
    static  Stmt* endStmtOfFunc(FunctionDecl *funcDecl) ;
    static  Stmt* endStmtOfCompoundStmt(Stmt *funcBody) ;


    static bool isReturnStmtClass(Stmt *stmt );
    /**计算语句列表中各语句是否为FallThrough，附带返回倒数第二条语句
     *
     * @param subStmtLs
     * @param negativeSecond
     * @return
     */
    static std::vector<bool>  subStmtIsFallThroughVec(const Stmt::child_range &subStmtLs ,Stmt* &negativeSecond ,SourceManager& SM, LangOptions& langOptions);
    /**语句是否有该属性
     *
     * @param stmt
     * @param attrKind
     * @return
     */
    static bool hasAttrKind(Stmt *stmt, attr::Kind attrKind);
    static void extractLineAndColumn(const clang::SourceManager& SM, const clang::SourceLocation& sourceLocation, int& line, int& column);
    static bool parentIsCompound(ASTContext* astContext, const Stmt* currentStmt);
    static bool parentClassEqual(ASTContext* astContext, const Stmt* stmt, Stmt::StmtClass targetClass);
    static bool parentKindIsSame(ASTContext *Ctx, const Stmt* stmt, const ASTNodeKind& kind);
    /**
     * 在声明语句 中 声明的变量个数
     * 比如 :
     * 输入 "int i;",  返回1
     * 输入 "float sum,x=0,y;", 返回3
     * 输入 非变量声明语句,  返回0
     * @param stmt
     * @return
     */
    static int varCntInVarDecl(DeclStmt* stmt);
    static void insertIncludeToFileStart(StringRef includeStmtText,FileID fileId, SourceManager &SM, const std::shared_ptr<Rewriter> rewriter_ptr,bool& insertResult);
    static void insertIncludeToFileStartByLoc(StringRef includeStmtText,SourceLocation Loc, SourceManager &SM, const std::shared_ptr<Rewriter> rewriter_ptr);
    static bool getSourceFilePathAtLoc(SourceLocation Loc, const SourceManager &SM,StringRef& fn);
    static void getSourceFilePathOfStmt(const Stmt *S, const SourceManager &SM,StringRef& fn);

    static FunctionDecl* findFuncDecByName(ASTContext *Ctx,std::string functionName);
    static std::vector<std::string> stmtLs2TextLs(std::vector<Stmt*> stmtVec, SourceManager & SM, const LangOptions & langOptions);
    static std::string getSourceTextBySourceRange(SourceRange sourceRange, SourceManager & sourceManager, const LangOptions & langOptions);



    static std::tuple<std::string,std::string>  get_FileAndRange_SourceText(const SourceRange &sourceRange,CompilerInstance& CI);
    static void printStmt(ASTContext &Ctx, CompilerInstance &CI, std::string tag, std::string title, clang::Stmt *expr,
                          bool printSourceText);
    static void printExpr(ASTContext &Ctx, CompilerInstance &CI, std::string tag, std::string title, clang::Expr *expr,
                          bool printSourceText = false);
    static void printDecl(ASTContext &Ctx, CompilerInstance &CI, std::string tag, std::string title, clang::Decl *decl,
                          bool printSourceText);
    static void  printSourceRange(int64_t nodeID,
      CompilerInstance& CI,
      std::string tag, std::string title,
      FileID fileId, const SourceRange &sourceRange,
      const char *topCategoryFieldName, const char *topCategoryName,
      const char *topCategoryEnumFieldName, int topCategoryEnum,
      const char *category1FieldName=NULL, int category1Enum=EMPTY_ENUM_VAL,
      const char *category2FieldName=NULL,int category2Enum=EMPTY_ENUM_VAL,
      bool printSourceText=false);
    static const int EMPTY_ENUM_VAL=-1;

};


#endif //CLANG_TUTOR_UTIL_H
