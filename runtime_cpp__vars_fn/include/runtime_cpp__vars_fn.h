
#ifndef runtime_cpp__vars_fn_H
#define runtime_cpp__vars_fn_H

#ifdef __cplusplus
// 主为c++, 书写真实定义
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

    //业务字段
    std::vector<_VarDecl> * _vdVec;
};
typedef struct __VarDeclLs _VarDeclLs;

//   函数左花括号后 插入 'void* _varLs_ptr=_init_varLs_inFn();'
_VarDeclLs *  _init_varLs_inFn(std::string srcFilePath, std::string funcName,int funcLBrc_line,int funcLBrc_column);


//结构体变量声明末尾 插入 'createVar(_varLs_ptr,"变量类型名",变量个数);'
void createVar(_VarDeclLs * _vdLs, std::string varTypeName,int varCnt);


//【销毁变量通知】 TODO  函数右花括号前 插入 'destroyVarLs_inFn(_varLs_ptr);'
void destroyVarLs_inFn(_VarDeclLs * _vdLs);

#endif

#ifndef __cplusplus
// 客为c, 指向c++函数
// 查找命令  objdump --syms  /fridaAnlzAp/clang-var/build/runtime_cpp__vars_fn/libclangPlgVar_runtime_cxx.a | grep Var
#define _init_varLs_inFn   _Z16_init_varLs_inFnNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEES4_ii
#define createVar          _Z9createVarP11__VarDeclLsNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEEi
#define destroyVarLs_inFn  _Z17destroyVarLs_inFnP11__VarDeclLs
#endif

#endif //runtime_cpp__vars_fn_H
