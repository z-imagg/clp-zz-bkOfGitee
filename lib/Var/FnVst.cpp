#include "Var/FnVst.h"

#include "clang/AST/AST.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "base/Util.h"
#include "Var/RangeHasMacroAstVst.h"
#include "Var/CollectIncMacro_PPCb.h"
#include <vector>

#include <fmt/core.h>
#include <iostream>
#include <clang/AST/ParentMapContext.h>

#include "base/MyAssert.h"

using namespace llvm;
using namespace clang;

bool FnVst::insertAfterFnLeftBrace__insertBeforeFnRightBrace(LocId funcLocId, SourceLocation funcBodyLBraceLoc , SourceLocation funcBodyRBraceLoc ){

//函数左花括号后 插入 '_VarDeclLs * _vdLs=_init_varLs_inFn("runtime_cpp__vars_fn/test_main__runtime_cpp__vars_fn.cpp","func1",7,14);'

    //region 构造插入语句
    std::string cStr_init=fmt::format(
            "_VarDeclLs * _vdLs=_init_varLs_inFn(\"{}\", \"{}\", {}, {}); /* 初始化函数变量列表, {}*/",
            funcLocId.filePath,
            funcLocId.funcName,
            funcLocId.line, funcLocId.column,
            funcLocId.to_string()
    );
    llvm::StringRef strRef_init(cStr_init);
    bool insertResult_init=mRewriter_ptr->InsertTextAfterToken(funcBodyLBraceLoc , strRef_init);
    //endregion


//【销毁变量通知】  函数右花括号前 插入 'destroyVarLs_inFn(_vdLs);'
    //region 构造插入语句
    std::string cStr_destroy=fmt::format(
            "destroyVarLs_inFn(_vdLs); /* 销毁函数变量列表, {}*/",
            funcLocId.filePath,
            funcLocId.funcName,
            funcLocId.to_string()
    );
    llvm::StringRef strRef_destroy(cStr_destroy);
    bool insertResult_destroy=mRewriter_ptr->InsertTextBefore(funcBodyRBraceLoc , strRef_destroy);
    //endregion


    //记录已插入语句的节点ID们以防重： 即使重复遍历了 但不会重复插入
    //用funcEnterLocIdSet的尺寸作为LocationId的计数器
    funcEnterLocIdSet.insert(funcLocId);

    return insertResult_init && insertResult_destroy;
}

bool FnVst::TraverseFunctionDecl(FunctionDecl *funcDecl) {
    //跳过非MainFile
    bool _LocFileIDEqMainFileID=Util::LocFileIDEqMainFileID(SM, funcDecl->getLocation());
    if(!_LocFileIDEqMainFileID){
        return false;
    }
    //跳过 default
    if(Util::funcIsDefault(funcDecl)){
        return false;
    }
    //跳过 无函数体
    bool hasBody=funcDecl->hasBody();
    if(!hasBody){
        return false;
    }
    //跳过 constexpr
    bool _isConstexpr = funcDecl->isConstexpr();
    if(_isConstexpr){
        return false;
    }

    //获得 函数体、左右花括号
    Stmt* body  = funcDecl->getBody();
    CompoundStmt* compoundStmt;
    SourceLocation funcBodyLBraceLoc,funcBodyRBraceLoc;
    Util::funcBodyIsCompoundThenGetLRBracLoc(body, compoundStmt, funcBodyLBraceLoc,funcBodyRBraceLoc);

    //跳过 函数体内无语句
    int stmtCntInFuncBody= Util::childrenCntOfCompoundStmt(compoundStmt);
    if(stmtCntInFuncBody<=0){
        return false;
    }

    //获取最后一条语句
    Stmt *endStmtOfFuncBody = Util::endStmtOfCompoundStmt(compoundStmt);

    //获取主文件ID,文件路径
    FileID mainFileId;
    std::string filePath;
    Util::getMainFileIDMainFilePath(SM,mainFileId,filePath);

    //获取函数名称
    const std::string &funcQualifiedName = funcDecl->getQualifiedNameAsString();
//  funcDecl->getNameAsString();
//  funcDecl->getName();

    //按照左右花括号，构建位置id，防止重复插入
    LocId funcBodyLBraceLocId=LocId::buildFor(filePath,funcQualifiedName, funcBodyLBraceLoc, SM);

    //获取返回类型
    const QualType funcReturnType = funcDecl->getReturnType();

    bool funcIsStatic = funcDecl->isStatic();
    bool funcIsInline = Util::funcIsInline(funcDecl);

    std::string verboseLogMsg=fmt::format("开发查问题日志funcIsStatic_funcIsInline:【{}:{}:{};funcQualifiedName】,funcIsStatic={},funcIsInline={}\n",filePath,funcBodyLBraceLocId.line,funcBodyLBraceLocId.column,funcIsStatic,funcIsInline);
    std::cout<<verboseLogMsg;

    return this->_Traverse_Func(//其中的insertAfter_X__funcEnter内Vst.funcEnterLocIdSet、funcLocId.locationId相互配合使得funcLocId.locationId作为funcLocId.srcFileId局部下的自增数
            funcIsStatic,funcIsInline,
            funcReturnType,
            false,
            endStmtOfFuncBody,
            funcBodyLBraceLoc,
            funcBodyRBraceLoc,
            funcBodyLBraceLocId,
            compoundStmt

    );
}


bool FnVst::_Traverse_Func(
        bool funcIsStatic,
        bool funcIsInline,
        QualType funcReturnType,
        bool isaCXXConstructorDecl,
        Stmt *endStmtOfFuncBody,
        SourceLocation funcBodyLBraceLoc,
        SourceLocation funcBodyRBraceLoc,
        LocId funcBodyLBraceLocId,
        CompoundStmt* compoundStmt
//  bool funcIsConstexpr,
//  bool hasBody,
//  int64_t funcDeclID,
//  Stmt *funcBodyStmt,
)
{

/////////////////////////对当前节点cxxMethodDecl|functionDecl做 自定义处理


    //region 插入 函数进入语句
    if(Util::LocIdSetNotContains(funcEnterLocIdSet, funcBodyLBraceLocId)){//若没有
//        Util::printStmt(*Ctx, CI, fmt::format("排查问题:{:x},",reinterpret_cast<uintptr_t> (&funcEnterLocIdSet)), funcBodyLBraceLocId.to_csv_line(), compoundStmt, true);

        //若 本函数还 没有 插入 函数进入语句，才插入。
        insertAfterFnLeftBrace__insertBeforeFnRightBrace(funcBodyLBraceLocId,funcBodyLBraceLoc,funcBodyRBraceLoc);
    }
//    }
    //endregion


///////////////////// 自定义处理 完毕


    return false;

}

