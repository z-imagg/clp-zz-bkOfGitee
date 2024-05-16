#!/bin/bash

LLVM15_HOME=/app/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4
export PATH=$LLVM15_HOME/bin:$PATH

PrjHm=/fridaAnlzAp/clang-var/
BldHm=$PrjHm/build__test_in

rm -fr $BldHm ; mkdir -p $BldHm ; cd $BldHm

[[ ! -f $PrjHm/build/bin/VarAlone ]] && bash $PrjHm/build.sh

$PrjHm/build/bin/VarAlone  $PrjHm/test_in/test_main.cpp

# 以clang编译命令-include 等效test_main.cpp中书写了'#include "runtime_cpp__vars_fn.h"'
clang++ -I $PrjHm/runtime_cpp__vars_fn/include/ -include runtime_cpp__vars_fn.h  -c   $PrjHm/test_in/test_main.cpp

clang++ -I $PrjHm/runtime_cpp__vars_fn/include   -c   $PrjHm/runtime_cpp__vars_fn/runtime_cpp__vars_fn.cpp

clang++ test_main.o runtime_cpp__vars_fn.o -o test_main.elf

./test_main.elf && echo test_main__ok