
#ifndef Var_CONSTANT_H
#define Var_CONSTANT_H
#include <string>

// constexpr  在编译时确定其值
class Constant{
public:
  constexpr static   const char*  PrgMsgStmt_funcIdAsmIns = "#pragma message(\"VarBE_inserted\")\n";

  //CollectIncMacro_PPCb::pragmaMsgFull决定了Constant::NameSpace_funcIdAsmIns的值的样式是 "命名空间:pragmaMessgae"
  constexpr static   const char* NameSpace_funcIdAsmIns = ":VarBE_inserted";

  //                                                           C语言(下标0==(int)false), C++(下标1==(int)true)
  constexpr static   const char* fnNameS__init_varLs[]={"_init_varLs_inFn__RtC00", "_init_varLs_inFn__RtCxx"};
  constexpr static   const char* fnNameS__createVar[]={"createVar__RtC00", "createVar__RtCxx"};
  constexpr static   const char* fnNameS__destroyVar[]={"destroyVarLs_inFn__RtC00", "destroyVarLs_inFn__RtCxx"};

};
#endif //Var_CONSTANT_H
