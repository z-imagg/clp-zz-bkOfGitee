
#ifndef runtime_c__vars_fn_H
#define runtime_c__vars_fn_H

// 本模块 runtime_c__vars_fn 只允许被C使用，而不允许被C++使用
#ifdef __cplusplus
// 本模块若被C++使用, 则报错以终止编译
#error "[ERROR] runtime_c__vars_fn must be used as C not C++"
#else
// 本模块必须被C使用

#include "list.h" //#include <vector>
#include "sds.h" // #include <string>

struct __VarDecl{


    //业务字段
    sds varTypeName;
    int varCnt;
};
typedef struct __VarDecl _VarDecl;

struct __VarDeclLs{
    //定位字段
    sds srcFilePath;
    sds funcName;
    int funcLBrc_line;
    int funcLBrc_column;

    //业务字段
    list_t* _vdVec; // std::vector<_VarDecl> * _vdVec;
};
typedef struct __VarDeclLs _VarDeclLs;

//   函数左花括号后 插入 'void* _varLs_ptr=_init_varLs_inFn__RtC00();'
_VarDeclLs *  _init_varLs_inFn__RtC00(sds srcFilePath, sds funcName, int funcLBrc_line, int funcLBrc_column);


//结构体变量声明末尾 插入 'createVar__RtC00(_varLs_ptr,"变量类型名",变量个数);'
void createVar__RtC00(_VarDeclLs * _vdLs, sds varTypeName, int varCnt);


//【销毁变量通知】    函数右花括号前 插入 'destroyVarLs_inFn__RtC00(_varLs_ptr);'
void destroyVarLs_inFn__RtC00(_VarDeclLs * _vdLs);

#endif  //__cplusplus判断结束

#endif //runtime_c__vars_fn_H
