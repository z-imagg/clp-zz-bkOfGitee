#  Release版本libCTk.so 运行崩溃 原因确定 
> 结论: Release版本的 libCTk.so 崩溃原因是:    函数 get_FileAndRange_SourceText 的入参CI 值是 NULL, 从而导致崩溃.

> 具体分析过程如下: 
##  Release版本libCTk.so 运行崩溃

```bash

mkdir /pubx/clang-tutor/build/
#Release版so会崩溃:
cmake -DCMAKE_BUILD_TYPE=Release ..
make
ls -lh /pubx/clang-tutor/build/lib/libCTk.so
# -rwxrwxr-x 1 zz zz 874K  7月 24 13:13 /pubx/clang-tutor/build/lib/libCTk.so
/app/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4//bin/clang -v    -Xclang   -load -Xclang /pubx/clang-tutor/build/lib/libCTk.so  -Xclang   -add-plugin -Xclang  CTk    -c /pubx/clang-ctk/t_clock_tick/test_main.cpp

#崩溃:
#查看，文件路径:/pubx/clang-ctk/t_clock_tick/test_main.cpp,mainFileId:1,frontendOptions.ProgramAction:13，Ctx.TUKind:0
#查看，复制文件路径:/pubx/clang-ctk/t_clock_tick/test_main.cpp到,文件路径:/tmp//pubx/clang-ctk/t_clock_tick/test_main.cpp_1690182594978
#提示，开始处理编译单元,文件路径:/pubx/clang-ctk/t_clock_tick/test_main.cpp,CTkAstConsumer:0x56018ad402b0,mainFileId:1
#查看,TranslationUnitDecl.decls.j,nodeID:534,文件路径、坐标:</pubx/clang-ctk/t_clock_tick/test_main.cpp:2:1, line:18:1>,getDeclKindName:Namespace,getKind:16,mainFileId:1,fileId:1
#Stack dump:
#0.	Program arguments: /app/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4//bin/clang -v -Xclang -load -Xclang /pubx/clang-tutor/build/lib/libCTk.so -Xclang -add-plugin -Xclang CTk -c /pubx/clang-ctk/t_clock_tick/test_main.cpp
#1.	<eof> parser at end of file
#0 0x00005601846d7053 llvm::sys::PrintStackTrace(llvm::raw_ostream&, int) (/app/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4//bin/clang+0x2f20053)
#1 0x00005601846d4f4e llvm::sys::RunSignalHandlers() (/app/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4//bin/clang+0x2f1df4e)
#2 0x00005601846d64e3 llvm::sys::CleanupOnSignal(unsigned long) (/app/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4//bin/clang+0x2f1f4e3)
#3 0x000056018464ca10 CrashRecoverySignalHandler(int) CrashRecoveryContext.cpp:0:0
#4 0x00007f7651a42520 (/lib/x86_64-linux-gnu/libc.so.6+0x42520)
#注意看 #5 在这里崩溃了，估计这里的入参 有指针是NULL，  但是由于现在是release版本 没有局部变量符号信息，即使gdb 也看不到哪个入参是NULL。因此要构建Release版本且有带局部变量名字的调试信息
#5 0x00007f7651d99bd8 Util::get_FileAndRange_SourceText[abi:cxx11](clang::SourceRange const&, clang::CompilerInstance&) (/pubx/clang-tutor/build/lib/libCTk.so+0x2fbd8)
#6 0x00007ffc89ef7e20 

```

## 构建Release版本且有带局部变量名字的调试信息 以定位问题
> GCC  调试信息不同级别选项 ：
- `-g1`：生成最基本的调试信息，包括变量名称和行号信息。
- `-g2`：除了基本的调试信息外，还包括局部变量的类型信息。
- `-g3`：除了 `-g2` 提供的信息外，还包括宏定义和宏展开的内容。

> 这里选择g2
```bash
#构建带调试信息的Release版本, 以排查问题:
LIB_CTk_CXX_FLAGS=" -fno-omit-frame-pointer -Wall   -O0    -g2"
cmake -DCMAKE_VERBOSE_MAKEFILE=True -DCMAKE_BUILD_TYPE=Release   -DCMAKE_CXX_FLAGS="$LIB_CTk_CXX_FLAGS"   -DCMAKE_C_FLAGS="$LIB_CTk_CXX_FLAGS"  ..
make
ls -lh /pubx/clang-tutor/build/lib/libCTk.so
# -rwxrwxr-x 1 zz zz 27M  7月 24 15:03 /pubx/clang-tutor/build/lib/libCTk.so

```

> 直接运行依然崩溃

> 下面用gdb运行，并找出崩溃时 函数入参们、局部变量们 中 谁是空指针.
```bash
cat > ./gdb_script.gs << 'EOF'
run
#运行后崩溃: Program received signal SIGSEGV, Segmentation fault.

#此时崩溃了
#查看崩溃时的调用栈
backtrace
#第一个栈帧是: get_FileAndRange_SourceText

#查看第一个栈帧的入参们: 看有没有指针变量是NULL
info args
#CI = @0x0: <incomplete type>  
#很明显入参CI是NULL, 注意CI类型是CompilerInstance&, 是引用类型，并非显示指针，但依然可以为NULL。
#即 Release版本的 libCTk.so 崩溃原因是:    函数 get_FileAndRange_SourceText 的入参CI 值是 NULL.

#查看局部变量们: 看有没有指针变量是NULL
info locals

#quit
EOF

gdb --quiet --command=./gdb_script.gs  --args /app/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4//bin/clang -v    -Xclang   -load -Xclang /pubx/clang-tutor/build/lib/libCTk.so  -Xclang   -add-plugin -Xclang  CTk    -c /pubx/clang-ctk/t_clock_tick/test_main.cpp

```

## 作为比对 看一下: Release+g1

```bash
 LIB_CTk_CXX_FLAGS=" -fno-omit-frame-pointer -Wall   -O0    -g1"
cmake -DCMAKE_VERBOSE_MAKEFILE=True -DCMAKE_BUILD_TYPE=Release   -DCMAKE_CXX_FLAGS="$LIB_CTk_CXX_FLAGS"   -DCMAKE_C_FLAGS="$LIB_CTk_CXX_FLAGS"  ..
make
ls -lh /pubx/clang-tutor/build/lib/libCTk.so
# -rwxrwxr-x 1 zz zz 4.6M  7月 24 14:50 lib/libCTk.so

```
> gcc 调试信息不同级别选项 产生的 libCTk.so文件大小对比:   
>- g1 : 4.6MB,  
>- g2 : 27MB

>很合理, g2 比 g1 大.