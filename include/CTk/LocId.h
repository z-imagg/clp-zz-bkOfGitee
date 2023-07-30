
#ifndef UniqueNode_H
#define UniqueNode_H


#include "clang/AST/AST.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "Util.h"
#include <fmt/core.h>

using namespace clang;

#include <iostream>
#include <clang/AST/ParentMapContext.h>

/**
std::set : 排序, 去重

std::unordered_set : 无排序,  去重。  这是这里的选择
 */

class LocId{
public:
    static LocId buildFor(std::string fp, const SourceLocation funcDeclBeginLoc, const clang::SourceManager& SM){
//      const SourceLocation &funcDeclBeginLoc = funcDecl->getBeginLoc();
      int line;
      int column;
//      Decl::Kind kind = funcDecl->getKind();
//      Stmt::StmtClass xx = funcDecl->getBody()->getStmtClass();
      Util::extractLineAndColumn(SM,funcDeclBeginLoc,line,column);
      return LocId(fp,line,column);
    }

    std::string to_string(){
      fmt::format("{}:{},{}",filePath,line,column);
    }
public:
//    Decl::Kind declKind;
//    Stmt::StmtClass stmtClass;
    int line;
    int column;
    std::string filePath;
    LocId(
//            Decl::Kind declKind, Stmt::StmtClass stmtClass,
            std::string filePath,int line, int column)
    :
//    declKind(declKind),
//    stmtClass(stmtClass),
      filePath(filePath),
      line(line),
    column(column)
    {

    }


    // 重写哈希函数
    size_t operator()(const LocId& that) const {
      // 同一个文件的 在同一个链条（hash）
      return std::hash<std::string>()(that.filePath) ;
    }

    // 重写相等操作符
    bool operator==(const LocId& that) const {
      return
//      this->stmtClass== that.stmtClass &&
      this->line== that.line
       && this->column== that.column
       && this->filePath== that.filePath
      ;
    }




};
#endif //UniqueNode_H
