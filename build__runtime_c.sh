#!/bin/bash

#【描述】
#【依赖】
#【术语】
#【备注】

#使用真实编译器(撤销cmd-wrap)
CmdWrap_Rm_F="/app/cmd-wrap/script/remove_interceptor.sh"
[[ -f $CmdWrap_Rm_F ]] && bash $CmdWrap_Rm_F

source /app/bash-simplify/_importBSFn.sh
_importBSFn "assertFileExisted.sh"
_importBSFn "cdCurScriptDir.sh"
_importBSFn "git_Clone_SwitchTag.sh"
_importBSFn "cmakeInstall.sh"
_importBSFn "link_CppPrj_includeDir_to.sh"


PrjHm=/fridaAnlzAp/clang-var/runtime_c__vars_fn
BUILD_HOME=$PrjHm/build/
( mkdir -p $BUILD_HOME && rm -fr $BUILD_HOME/* ;) || exit $?

{
#构建的Release版本 : build_release_0.sh
# LIB_Var_CXX_FLAGS="  "
# -rwxrwxr-x 1 zz zz 874K  7月 24 13:13 /bal/clang-add-funcIdAsm/build/lib/libVar.so

#构建的Release版本,带调试信息(变量名称和行号),  当出现问题方便排查: build_release_1.sh
LIB_Var_CXX_FLAGS=" -fno-omit-frame-pointer -Wall   -O0    -g1"
# -rwxrwxr-x 1 zz zz 4.6M  7月 24 14:50 lib/libVar.so

#构建的Release版本,带调试信息(变量名称和行号、局部变量),  当出现问题方便排查:  build_release_2.sh
# LIB_Var_CXX_FLAGS=" -fno-omit-frame-pointer -Wall   -O0    -g2"
# -rwxrwxr-x 1 zz zz 27M  7月 24 15:03 /bal/clang-add-funcIdAsm/build/lib/libVar.so

cmake -S $PrjHm -B $BUILD_HOME  -DCMAKE_BUILD_TYPE=Release   -DCMAKE_CXX_FLAGS="$LIB_Var_CXX_FLAGS"   -DCMAKE_C_FLAGS="$LIB_Var_CXX_FLAGS"    && \
#-DCMAKE_VERBOSE_MAKEFILE=True
( cd $BUILD_HOME && make -j2 ;) && \
true ;}

$BUILD_HOME/runtime_c__test_main && echo "TEST_OK"
## 构建Release版本且有带局部变量名字的调试信息 以定位问题
#> GCC  调试信息不同级别选项 ：
#- `-g1`：生成最基本的调试信息，包括变量名称和行号信息。
#- `-g2`：除了基本的调试信息外，还包括局部变量的类型信息。
#- `-g3`：除了 `-g2` 提供的信息外，还包括宏定义和宏展开的内容。
