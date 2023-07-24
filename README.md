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

###  定位崩溃的直接原因

> 注意仔细看一下调用栈帧:
```bash
#0  Util::get_FileAndRange_SourceText[abi:cxx11](clang::SourceRange const&, clang::CompilerInstance&) (sourceRange=..., CI=...) at /pubx/clang-ctk/lib/CTk/Util.cpp:111
#1  0x000055555e6dbd88 in ?? ()
#2  0x00000000000002ee in ?? ()
#3  0x00007fffffffb180 in ?? ()
#4  0x00007ffff7be634f in CTkVst::processStmt (this=0x55555e6e52b0, stmt=0x55555e7636f0, whoInserted=<optimized out>) at /pubx/clang-ctk/lib/CTk/CTkVst.cpp:172
```
> 疑点: 
> 
1.  #1 #2 #3 为何没名字?

2.  分析源码得知：#4 根本没有调用过 #0  . 详细分析如下:

```cpp
//CTkVst.cpp
bool CTkVst::processStmt(Stmt *stmt,const char* whoInserted){
//...
  Util::getSourceFilePathOfStmt(stmt, SM, fn); //CTkVst.cpp:172.  即行即 #4
//...
}
```
```cpp
//Util.cpp
bool Util::getSourceFilePathOfStmt(const Stmt *S, const SourceManager &SM,StringRef& fn) {
  SourceLocation Loc = S->getBeginLoc();
  Util::getSourceFilePathAtLoc(Loc,SM,fn);  //  #4调用了此函数，但此函数有问题。
  //此函数的问题是：  本函数声明了返回bool, 但这里并没有返回值语句.
  //当 #4 执行到这里的时, 没有返回语句，自然是继续执行，继续执行什么？ 
  // 继续执行 本文件 （即 Util.o）中 本函数末尾的下一条机器指令，到此已经很危险了，
  // 如果 编译器 没有在 本函数末尾 和 下一个函数开头 塞入 一些指令的话，这条指令肯定是 Util.o 中 本函数末尾的下一个函数的开头 。
  // 所以结果很不确定，这相当于是乱跳了，执行到别的文件中都有可能。
  // 这里 不确定的乱跳 最终跳到了 同文件的下面一个无关函数 get_FileAndRange_SourceText 的入口处，但显然  大概率 入参不对，
  // 调试结果 支撑这一点。 分析源码可知， get_FileAndRange_SourceText 的 入参 CI 不可能是NULL， 但却是NULL。
  // 此调用栈 中 的 没名字的函数 #1 #2 #3 应该就是 乱跳 的具体过程。  即 疑点1 有了大概对应。
  // 问题调查结束，
  // Release版libCTk.so作为clang插件运行崩溃直接原因： 崩溃调用栈中 中 下层#4栈帧 的源码 调用了一个 残缺函数(缺少return语句), 导致 执行流 不确定的乱跳，最终 跳到 某函数（#0中的函数）入口，因入参大概率不对（入参CI指针是NULL），引发空指针导致崩溃（Segmentation fault）
}

//同文件下 有崩溃处的函数:
std::tuple<std::string,std::string>  Util::get_FileAndRange_SourceText(const SourceRange &sourceRange,CompilerInstance& CI){
//...
}
  
```

> 此分析，进一步说明：  
> 所谓 Debug是 :
>1. 编译器 生成 调试信息
>2. 编译器 自发的解决 源码中 可能存在的问题， 即 编译器 并不止 如实地  翻译源码，还帮忙补漏。
>>比如 以上 编译器 主动 对 以上  残缺函数(缺少return语句) getSourceFilePathOfStmt 默认 加一条 返回语句, 以避免大问题出现。
>>
>>具体来说，应该是 自动的增加了 大量的 gcc 选项

> 显然，所谓Release是：
>1. 编译器 不生成 调试信息
>2. 编译器 仅仅 如实地 翻译源码，不补充任何源码中没表达的内容。

> 当没有编译器的补漏，便发生了这里描述的问题。

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