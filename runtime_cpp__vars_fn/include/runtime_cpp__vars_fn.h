
#ifndef runtime_cpp__vars_fn_H
#define runtime_cpp__vars_fn_H

#include <vector>
#include <string>
struct __VarDecl{
    std::string varTypeName;
    int varCnt;
};

typedef struct __VarDecl _VarDecl;

//TODO  函数左花括号后 插入 'void* _varLs_ptr=_init_varLs_inFn();'
std::vector<_VarDecl> *  _init_varLs_inFn(char* srcFilePath,char* funcName,int funcLBrc_line,int funcLBrc_column);


//结构体变量声明末尾 插入 'createVar(_varLs_ptr,"变量类型名",变量个数);'
void createVar(void* _varLs_ptr, char* varTypeName,int varCnt);


//【销毁变量通知】 TODO  函数右花括号前 插入 'destroyVarLs_inFn(_varLs_ptr);'
void destroyVarLs_inFn(void* _varLs_ptr);
#endif //runtime_cpp__vars_fn_H
