```bash
sudo apt  install cmake  # version 3.22.1-1ubuntu1.22.04.1
```

```bash
##编译运行 尝试中

#https://github.com/llvm/llvm-project/releases?page=2
wget https://github.com/llvm/llvm-project/releases/download/llvmorg-15.0.0/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4.tar.xz
xz -d clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4.tar.xz
sudo mkdir /llvm_release_home/
sudo chown -R z.z /llvm_release_home
tar -xvf clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4.tar -C /llvm_release_home/

```

```bash
export PATH=/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/bin/:$PATH
export LD_LIBRARY_PATH=/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/lib
```

```bash
export Clang_DIR=/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/
export CLANG_TUTOR_DIR=/pubx/source_code_rewrite/clang-plugins-demo/clang-tutor/
cd $CLANG_TUTOR_DIR
mkdir build
cd build
cmake -DCT_Clang_INSTALL_DIR=$Clang_DIR $CLANG_TUTOR_DIR/HelloWorld/
make
# Run the plugin
$Clang_DIR/bin/clang -cc1 -load ./libHelloWorld.{so|dylib} -plugin hello-world $CLANG_TUTOR_DIR/test/HelloWorld-basic.cpp

#总是报错： libstdc++找不到
```



---------------
>换个办法
```bash
sudo apt install clang-15 -y
readlink -f  `which clang++-15`
#/usr/lib/llvm-15/bin/clang

export PATH=/usr/lib/llvm-15/bin/:$PATH
export Clang_DIR=/usr/lib/llvm-15/

#报错："/usr/lib/llvm-15/lib/libclangBasic.a"
```
