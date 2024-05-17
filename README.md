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
  ```
参考: https://www.ibm.com/docs/en/xl-c-and-cpp-linux/16.1.0?topic=cla-running-user-defined-actions-by-using-clang-plug-ins





##### clang插件其他运行方法
- `-add-plugin` 尝试 编译、链接、本插件， 本插件 正常修改源文件,  正常编译链接 输出 test_main.elf

      不确定 编译、链接、本插件 之间的先后次序
  ```shell
  clang++  $Clang_VFIRPlugin_run   $Clang_Varlugin_run  $test_main_cpp_F  -o test_main.elf -mllvm -print-after-all 2>&1 | gedit -
  ```
