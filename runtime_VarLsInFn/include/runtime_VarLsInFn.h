
#ifndef runtime_VarLsInFn_H
#define runtime_VarLsInFn_H

//TODO  函数左花括号后 插入 'void* _varLs_ptr=_init_varLs_inFn();'
void* _init_varLs_inFn();


//结构体变量声明末尾 插入 'createVar(_varLs_ptr,"变量类型名",变量个数);'
void createVar(void* _varLs_ptr, char* varTypeName,int varCnt);


//【销毁变量通知】 TODO  函数右花括号前 插入 'destroyVarLs_inFn(_varLs_ptr);'
void destroyVarLs_inFn(void* _varLs_ptr);
#endif //runtime_VarLsInFn_H
