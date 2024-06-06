
#ifndef runtime_cpp__vars_fn_H
#define runtime_cpp__vars_fn_H

/* frida_js中的 术语
TmPnt == TimePoint
*/ 

// 本模块runtime_cpp__vars_fn 只允许被C++使用，而不允许被C使用
#ifndef __cplusplus
// 本模块若被C使用, 则报错以终止编译
#error "[ERROR] runtime_cpp__vars_fn must be used as C++ not C @runtime_cpp__vars_fn.h"
#else
// 本模块必须被C++使用
#include <vector>
#include <string>
struct __VarDecl{


    //业务字段
    std::string varTypeName;
    int varCnt;
};
typedef struct __VarDecl _VarDecl;

struct __VarDeclLs{
    //定位字段
    std::string srcFilePath;
    std::string funcName;
    int funcLBrc_line;
    int funcLBrc_column;

    //这些变量 所属 时刻点
    int TmPnt;

    //业务字段
    std::vector<_VarDecl> * _vdVec;
};
typedef struct __VarDeclLs _VarDeclLs;

//   函数左花括号后 插入 'void* _varLs_ptr=_init_varLs_inFn__RtCxx();'
_VarDeclLs *  _init_varLs_inFn__RtCxx(std::string srcFilePath, std::string funcName, int funcLBrc_line, int funcLBrc_column);


//结构体变量声明末尾 插入 'createVar__RtCxx(_varLs_ptr,"变量类型名",变量个数);'
void createVar__RtCxx(_VarDeclLs * _vdLs, std::string varTypeName, int varCnt);


//【销毁变量通知】  函数右花括号前 插入 'destroyVarLs_inFn__RtCxx(_varLs_ptr);'
void destroyVarLs_inFn__RtCxx(_VarDeclLs * _vdLs);

#endif  //__cplusplus判断结束

#endif //runtime_cpp__vars_fn_H
