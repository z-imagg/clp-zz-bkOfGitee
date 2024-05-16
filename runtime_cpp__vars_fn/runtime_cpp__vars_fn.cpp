
#include "runtime_cpp__vars_fn.h"



#include <iostream>
#include <algorithm>


//函数左花括号后 插入 'void* _varLs_ptr=_init_varLs_inFn();'
std::vector<_VarDecl> *  _init_varLs_inFn(char* srcFilePath,char* funcName,int funcLBrc_line,int funcLBrc_column){
    std::vector<_VarDecl> *_vdVec = new std::vector<_VarDecl>();
    return _vdVec;
}


//结构体变量声明末尾 插入 'createVar(_varLs_ptr,"变量类型名",变量个数);'
void createVar(std::vector<_VarDecl> * _vdVec, char* varTypeName,int varCnt){

    _VarDecl vd;
    vd.varTypeName=varTypeName;
    vd.varCnt=varCnt;

    _vdVec->push_back(vd);

}


//【销毁变量通知】 函数右花括号前 插入 'destroyVarLs_inFn(_varLs_ptr);'
void destroyVarLs_inFn(std::vector<_VarDecl> *  _vdVec){

    long vdCnt = std::distance(_vdVec->begin(), _vdVec->end());
    std::cout<<"varDeclCnt="<<vdCnt<<std::endl;

    std::for_each(_vdVec->begin(), _vdVec->end(), [](const _VarDecl vd){
        std::cout<<"vd:{varTypeName="<<vd.varTypeName<<",varCnt="<<vd.varCnt<<"}"<<std::endl;
    });

    delete _vdVec;
}
