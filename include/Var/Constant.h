
#ifndef Var_CONSTANT_H
#define Var_CONSTANT_H
#include <string>

// constexpr  在编译时确定其值
class Constant{
public:
  constexpr static   const char*  PrgMsgStmt_funcIdAsmIns = "#pragma message(\"VarBE_inserted\")\n";

  //CollectIncMacro_PPCb::pragmaMsgFull决定了Constant::NameSpace_funcIdAsmIns的值的样式是 "命名空间:pragmaMessgae"
  constexpr static   const char* NameSpace_funcIdAsmIns = ":VarBE_inserted";

  constexpr static   const char* fnNameS__init_varLs[]={"_init_varLs_inFn__RtCxx","_init_varLs_inFn__RtC00"};
  constexpr static   const char* fnNameS__createVar[]={"createVar__RtCxx","createVar__RtC00"};
  constexpr static   const char* fnNameS__destroyVar[]={"destroyVarLs_inFn__RtCxx","destroyVarLs_inFn__RtCxx"};

};
#endif //Var_CONSTANT_H
