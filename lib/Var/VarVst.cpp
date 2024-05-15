#include "Var/VarVst.h"

#include "clang/AST/AST.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "base/Util.h"
#include "Var/RangeHasMacroAstVst.h"
#include "Var/CollectIncMacro_PPCb.h"
#include <vector>

#include <fmt/core.h>

using namespace clang;

#include <iostream>
#include <clang/AST/ParentMapContext.h>
#include <fmt/core.h>


using namespace llvm;
using namespace clang;

void printDeclStmtTree(DeclStmt *declStmt, int depth) {
    // 获取当前节点的声明
    Decl *decl = nullptr;
    if (auto *singleDecl = declStmt->getSingleDecl()) {
        decl = singleDecl;

    } else  {
        // 如果是一组声明，则选择第一个声明
        decl=* (declStmt->getDeclGroup().begin());
    }

    // 如果找到声明，则打印其名称
    if (decl) {
//        decl->getKind()
        Decl::Kind declKind = decl->getKind();
        std::string  msg=fmt::format("depth={}, kind={}, DeclKindName={}\n",depth,(int)declKind,decl->getDeclKindName());
//        llvm::errs() << msg ;
        std::cout<<msg;
    }

    // 打印当前节点的子节点
    for (auto *child : declStmt->children()) {
        if (auto *childDeclStmt = llvm::dyn_cast<DeclStmt>(child)) {
            printDeclStmtTree(childDeclStmt, depth + 1);
        } else {
            // 如果子节点不是 DeclStmt，则打印其名称
            Stmt::StmtClass childStmtClass = child->getStmtClass();
            std::string  msg=fmt::format("depth={}, StmtClass={}, StmtClassName={}\n",depth+1,(int)childStmtClass,child->getStmtClassName());
//            llvm::errs() << msg  ;
            std::cout<<msg;
        }
    }
}

bool VarVst::TraverseDeclStmt(DeclStmt* declStmt){
    printDeclStmtTree(declStmt,0);
    Stmt::StmtClass stmtClass = declStmt->getStmtClass();
    const char *stmtClassName = declStmt->getStmtClassName();

//    const Stmt::child_range &child = declStmt->children();
//    std::vector<const Stmt*> childLs(child.begin(), child.end());
//    const Stmt *&child0 = childLs.at(0);

//    const DeclStmt::decl_range &decls = declStmt->decls();
//    std::vector<const Decl*> declLs(decls.begin(), decls.end());
//    const Decl *&decl_0 = declLs.at(0);
    
    bool isSingleDecl = declStmt->isSingleDecl();
    Decl *p_singleDecl = declStmt->getSingleDecl();
    Decl &singleDecl = *p_singleDecl;

    Decl::Kind singleDeclKind = singleDecl.getKind();

    SourceLocation beginLoc,endLoc;
    beginLoc = declStmt->getBeginLoc();
    endLoc = declStmt->getEndLoc();

    //构造人类可读开始位置、结束位置、插入者 注释文本
    std::string hmTxtCmnt_whoInsrt_BE;
    Util::BE_Loc_HumanText(SM, beginLoc, endLoc, "test_VisitDeclStmt", hmTxtCmnt_whoInsrt_BE);
    std::cout<<hmTxtCmnt_whoInsrt_BE<<"\n";
    return true;
}