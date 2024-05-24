
#ifndef runtime_cpp__vars_fn_H
#define runtime_cpp__vars_fn_H

// 始终保持 为 C++函数，而非C函数
#ifndef __cplusplus
extern "C++" {
#endif

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

#ifndef __cplusplus
}
#endif

#endif //runtime_cpp__vars_fn_H
