#include "Var/VarVst.h"

#include "clang/AST/AST.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "base/Util.h"
#include "Var/RangeHasMacroAstVst.h"
#include "Var/CollectIncMacro_PPCb.h"

using namespace clang;

#include <iostream>
#include <clang/AST/ParentMapContext.h>
#include <fmt/core.h>


using namespace llvm;
using namespace clang;






bool VarVst::TraverseDeclStmt(DeclStmt* declStmt){



    SourceLocation beginLoc,endLoc;
    beginLoc = declStmt->getBeginLoc();
    endLoc = declStmt->getEndLoc();

    //构造人类可读开始位置、结束位置、插入者 注释文本
    std::string hmTxtCmnt_whoInsrt_BE;
    Util::BE_Loc_HumanText(SM, beginLoc, endLoc, "test_VisitDeclStmt", hmTxtCmnt_whoInsrt_BE);
    std::cout<<hmTxtCmnt_whoInsrt_BE;
    return true;
}