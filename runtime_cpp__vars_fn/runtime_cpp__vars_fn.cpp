
#include "runtime_cpp__vars_fn.h"


#ifdef __cplusplus
// 主为c++, 书写真实定义

#include <iostream>
#include <algorithm>
#include <numeric>

//函数左花括号后 插入 'void* _varLs_ptr=_init_varLs_inFn();'
_VarDeclLs *  _init_varLs_inFn(std::string srcFilePath, std::string funcName,int funcLBrc_line,int funcLBrc_column){
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


//结构体变量声明末尾 插入 'createVar(_varLs_ptr,"变量类型名",变量个数);'
void createVar(_VarDeclLs *vdLs, std::string varTypeName,int varCnt){

    _VarDecl vd;
    vd.varTypeName=varTypeName;
    vd.varCnt=varCnt;

    vdLs->_vdVec->push_back(vd);

}


//【销毁变量通知】 函数右花括号前 插入 'destroyVarLs_inFn(_varLs_ptr);'
void destroyVarLs_inFn(_VarDeclLs *vdLs){
    std::vector<_VarDecl> *_vdVec = vdLs->_vdVec;

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
    std::cout << vdLs->srcFilePath << ":" << vdLs->funcLBrc_line << ":" << vdLs->funcLBrc_column << ",varDeclGroupCnt=" << varDeclGroupCnt << ",varCntSum="<< varCntSum << std::endl;

    std::for_each(_vdVec->begin(), _vdVec->end(), [](const _VarDecl vd){
        std::cout<<"vd:{varTypeName="<<vd.varTypeName<<",varCnt="<<vd.varCnt<<"}"<<std::endl;
    });

    //delete:HeapObj_vdVec
    delete vdLs->_vdVec;  vdLs->_vdVec= nullptr;
    //delete:HeapObj_vdLs
    delete vdLs;

}


#endif