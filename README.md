-Xclang   -load -Xclang /fridaAnlzAp/clang-voidFnEndInsertRet/build/lib/libVFIRPlugin.so  -Xclang   -add-plugin -Xclang  VFIRPlugin





#### 术语
- VFIR == Void Function Insert Return == 'void函数末尾无return时候，补充return语句'

#### 插件运行举例

```shell
LLVM15HOME=/app/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4
export PATH=$LLVM15HOME/bin:$PATH
test_main_cpp_F=/fridaAnlzAp/clang-var/test_in/test_main.cpp
```

###### 写作CXXFLAGS、CFLAGS以在clang编译命令中运行插件VFIRPlugin
```shell
Clang_VFIRPlugin_run=" -Xclang   -load -Xclang /fridaAnlzAp/clang-voidFnEndInsertRet/build/lib/libVFIRPlugin.so  -Xclang   -add-plugin -Xclang  VFIRPlugin "

```

###### 写作CXXFLAGS、CFLAGS以在clang编译命令中运行插件VFIRPlugin
```shell
Clang_Varlugin_run=" -Xclang   -load -Xclang /fridaAnlzAp/clang-var/build/lib/libVarPlugin.so -Xclang   -add-plugin -Xclang  VarPlugin   "

```

##### clang插件主要运行方法

###### 一般运行插件
-  `-add-plugin` 尝试 编译、本插件， 本插件 正常修改源文件， 正常编译 输出 test_main.o

       不确定 编译、本插件 之间的先后次序
  ```shell
clang++  $Clang_VFIRPlugin_run   $Clang_Varlugin_run -c  $test_main_cpp_F 
#两个插件放在一起运行，第二个插件修改源码文本时报错
#6 0x0000651c02920df6 clang::Rewriter::InsertText(clang::SourceLocation, llvm::StringRef, bool, bool) (/app/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/bin/clang-15+0x5ad0df6)
#7 0x00007858cf2a2960 FnVst::insertBefore_Return(LocId, clang::SourceLocation) /fridaAnlzAp/clang-voidFnEndInsertRet/lib/VFIR/FnVst.cpp:0:0
  ```




因此，暂时只能分两次运行这两个插件

##### 插件使用步骤(请按此步骤写入cmd-wrap中)

1. 分两次运行插件，以修改源码
  ```shell
clang++  $Clang_VFIRPlugin_run  -c  $test_main_cpp_F 
clang++  $Clang_Varlugin_run    -c  $test_main_cpp_F 
```

2. 最后， 编译命令中加入'-include runtime_cpp__vars_fn.h', 再正常编译修改后源码
```shell

PrjHm=/fridaAnlzAp/clang-var/

# 以clang编译命令-include 等效test_main.cpp中书写了'#include "runtime_cpp__vars_fn.h"'
clang++ -I $PrjHm/runtime_cpp__vars_fn/include/ -include runtime_cpp__vars_fn.h  -c   $PrjHm/test_in/test_main.cpp

```

3. 链接时，加入runtime_cpp__vars_fn.o
```shell

clang++ -I $PrjHm/runtime_cpp__vars_fn/include   -c   $PrjHm/runtime_cpp__vars_fn/runtime_cpp__vars_fn.cpp

#链接时，加入runtime_cpp__vars_fn.o
clang++ test_main.o runtime_cpp__vars_fn.o -o test_main.elf

./test_main.elf && echo test_main__ok
```