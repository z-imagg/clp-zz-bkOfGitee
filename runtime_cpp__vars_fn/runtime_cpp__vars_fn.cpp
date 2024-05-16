
#include "runtime_cpp__vars_fn.h"
#include <string>
struct _VarDecl{
    std::string varTypeName;
    int varCnt;
};

//typedef struct _VarDecl VarDec;

#include <vector>
#include <iostream>
#include <algorithm>


//TODO  函数左花括号后 插入 'void* _varLs_ptr=_init_varLs_inFn();'
void* _init_varLs_inFn(){
    std::vector<struct _VarDecl> *_vdVec = new std::vector<struct _VarDecl>();
    static_cast<void*>(_vdVec);
//    return (void*)_vdVec;
    return static_cast<void*>(_vdVec);
}


//结构体变量声明末尾 插入 'createVar(_varLs_ptr,"变量类型名",变量个数);'
void createVar(void* _varLs_ptr, char* varTypeName,int varCnt){
    std::vector<struct _VarDecl> *_vdVec = static_cast<std::vector<struct _VarDecl> *>(_varLs_ptr);

    struct _VarDecl vd;
    vd.varTypeName=varTypeName;
    vd.varCnt=varCnt;

    _vdVec->push_back(vd);

}


//【销毁变量通知】 TODO  函数右花括号前 插入 'destroyVarLs_inFn(_varLs_ptr);'
void destroyVarLs_inFn(void* _varLs_ptr){
    std::vector<struct _VarDecl> *_vdVec = static_cast<std::vector<struct _VarDecl> *>(_varLs_ptr);

    long vdCnt = std::distance(_vdVec->begin(), _vdVec->end());
    std::cout<<"varDeclCnt="<<vdCnt<<std::endl;

    std::for_each(_vdVec->begin(), _vdVec->end(), [](const struct _VarDecl vd){
        std::cout<<"vd:{varTypeName="<<vd.varTypeName<<",varCnt="<<vd.varCnt<<"}"<<std::endl;
    });
}
