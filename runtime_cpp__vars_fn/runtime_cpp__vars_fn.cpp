
#include "runtime_cpp__vars_fn.h"

// 本模块runtime_cpp__vars_fn 只允许被C++使用，而不允许被C使用
#ifndef __cplusplus
// 本模块若被C使用, 则报错以终止编译
#error "[ERROR] runtime_cpp__vars_fn must be used as C++ not C @runtime_cpp__vars_fn.cpp"
#else
// 本模块必须被C++使用

#include <iostream>
#include <algorithm>
#include <numeric>

//函数左花括号后 插入 'void* _varLs_ptr=_init_varLs_inFn__RtCxx();'
_VarDeclLs *  _init_varLs_inFn__RtCxx(std::string srcFilePath, std::string funcName, int funcLBrc_line, int funcLBrc_column){
    //new:HeapObj_vdLs
    _VarDeclLs *vdLs = new _VarDeclLs;
    //new:HeapObj_vdVec
    vdLs->_vdVec=new std::vector<_VarDecl>();

    vdLs->srcFilePath=srcFilePath;
    vdLs->funcName=funcName;
    vdLs->funcLBrc_line=funcLBrc_line;
    vdLs->funcLBrc_column=funcLBrc_column;

    return vdLs;
}


//结构体变量声明末尾 插入 'createVar__RtCxx(_varLs_ptr,"变量类型名",变量个数);'
void createVar__RtCxx(_VarDeclLs *_vdLs, std::string varTypeName, int varCnt){

    _VarDecl vd;
    vd.varTypeName=varTypeName;
    vd.varCnt=varCnt;

    _vdLs->_vdVec->push_back(vd);

}


//【销毁变量通知】 函数右花括号前 插入 'destroyVarLs_inFn__RtCxx(_varLs_ptr);'
void destroyVarLs_inFn__RtCxx(_VarDeclLs *_vdLs){
    std::vector<_VarDecl> *_vdVec = _vdLs->_vdVec;

    _VarDecl zero; zero.varCnt=0;

    const _VarDecl &sum_vd = std::accumulate(
    _vdVec->begin(),
    _vdVec->end(),
    zero,
[](const _VarDecl &a, const _VarDecl &b) {
      _VarDecl result;
      result.varCnt = a.varCnt + b.varCnt;
      return result;
    }
    );

    int varCntSum=sum_vd.varCnt;

    long varDeclGroupCnt = std::distance(_vdVec->begin(), _vdVec->end());
    std::cout << _vdLs->srcFilePath << ":" << _vdLs->funcLBrc_line << ":" << _vdLs->funcLBrc_column << ",varDeclGroupCnt=" << varDeclGroupCnt << ",varCntSum=" << varCntSum << std::endl;

    std::for_each(_vdVec->begin(), _vdVec->end(), [](const _VarDecl vd){
        std::cout<<"vd:{varTypeName="<<vd.varTypeName<<",varCnt="<<vd.varCnt<<"}"<<std::endl;
    });

    //delete:HeapObj_vdVec
    delete _vdLs->_vdVec; _vdLs->_vdVec= nullptr;
    //delete:HeapObj_vdLs
    delete _vdLs;

}


#endif  //__cplusplus判断结束