
#include "runtime_c__vars_fn.h"
#include <malloc.h>

// 本模块 runtime_c__vars_fn 只允许被C使用，而不允许被C++使用
#ifdef __cplusplus
// 本模块若被C++使用, 则报错以终止编译
#error "[ERROR] runtime_c__vars_fn must be used as C not C++"
#else
// 本模块必须被C使用


#define _NEW_(Type)  ( (Type*)malloc(sizeof(Type)) )
#define _DEL_(ptr)   free(ptr); ptr=NULL

#include "sds.h"
#include "list.h"

//函数左花括号后 插入 'void* _varLs_ptr=_init_varLs_inFn();'
_VarDeclLs *  _init_varLs_inFn(sds srcFilePath, sds funcName,int funcLBrc_line,int funcLBrc_column){
    //new:HeapObj_vdLs
    _VarDeclLs *vdLs = _NEW_(_VarDeclLs); // 分配 对象0 : _NEW_0  //new _VarDeclLs;
    //new:HeapObj_vdVec
    vdLs->_vdVec=list_new();//new std::vector<_VarDecl>();

    vdLs->srcFilePath=srcFilePath;
    vdLs->funcName=funcName;
    vdLs->funcLBrc_line=funcLBrc_line;
    vdLs->funcLBrc_column=funcLBrc_column;

    return vdLs;
}


//结构体变量声明末尾 插入 'createVar(_varLs_ptr,"变量类型名",变量个数);'
void createVar(_VarDeclLs *vdLs, sds varTypeName,int varCnt){

    _VarDecl *vd= _NEW_(_VarDecl);//分配 对象1 : _NEW_1
    vd->varTypeName=varTypeName;
    vd->varCnt=varCnt;

    list_node_t *node=list_node_new(vd);
    list_rpush(vdLs->_vdVec,node); // vdLs->_vdVec->push_back(vd);
}


//【销毁变量通知】 函数右花括号前 插入 'destroyVarLs_inFn(_varLs_ptr);'
void destroyVarLs_inFn(_VarDeclLs *vdLs){
    list_t* _vdVec = vdLs->_vdVec; // std::vector<_VarDecl>

    _VarDecl zero; zero.varCnt=0;

    int varCntSum=0;

    list_node_t *nodeK;
    list_iterator_t *it = list_iterator_new(_vdVec, LIST_HEAD);
    while ((nodeK = list_iterator_next(it))) {
      _VarDecl* vdK=(_VarDecl*)(nodeK->val); //这不是c++，这是c，无类型信息，只能做危险的强制类型转换
      printf("vd:{varTypeName=%s,varCnt=%d}\n",vdK->varTypeName,vdK->varCnt);
      varCntSum += vdK->varCnt;
      _DEL_(vdK);//释放 对象1 : _DEL_1
    }
    //释放迭代器(不释放链表自身)
    list_iterator_destroy(it);


    long varDeclGroupCnt = _vdVec->len; //std::distance(_vdVec->begin(), _vdVec->end());
    printf( "%s:%d:%d,varDeclGroupCnt=%d,varCntSum=%d\n", vdLs->srcFilePath ,  vdLs->funcLBrc_line , vdLs->funcLBrc_column ,  varDeclGroupCnt,varCntSum ) ;

    list_destroy(vdLs->_vdVec); // 释放 对象0 : _DEL_0  // delete vdLs->_vdVec;  vdLs->_vdVec= nullptr;
    _DEL_(vdLs); // delete vdLs;

}

#endif  //__cplusplus判断结束
