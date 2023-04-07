```bash

##编译运行 尝试中

#https://github.com/llvm/llvm-project/releases?page=2
wget https://github.com/llvm/llvm-project/releases/download/llvmorg-15.0.0/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4.tar.xz
xz -d clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4.tar.xz
sudo mkdir /llvm_release_home/
sudo chown -R z.z /llvm_release_home
tar -xvf clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4.tar -C /llvm_release_home/



sudo docker pull kernelci/clang-15:latest
sudo docker  run   --security-opt seccomp=unconfined   --name centos7_clang15Plugin_devEnv -itd -v /pubx/:/pubx/   kernelci/clang-15:latest
sudo docker exec -it  centos7_clang15Plugin_devEnv  bash



```

```bash

#export Clang_DIR=/usr/lib/llvm-15/  #注意怪在这里：应该用这个llvm才对，但是用这个会报错：没有*Basic.a库
export Clang_DIR=/pubx/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/

export Clang_DIR=/pubx/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/
export CLANG_TUTOR_DIR=/pubx/source_code_rewrite/clang_plugin_demo/clang-tutor/
cd $CLANG_TUTOR_DIR
mkdir build
cd build
cmake -DCT_Clang_INSTALL_DIR=$Clang_DIR $CLANG_TUTOR_DIR/HelloWorld/
make
##正常编译
$Clang_DIR/bin/clang -cc1 -load ./libHelloWorld.so -plugin hello-world $CLANG_TUTOR_DIR/test/HelloWorld-basic.cpp

#(clang-tutor)  file: /pubx/source_code_rewrite/clang_plugin_demo/clang-tutor//test/HelloWorld-basic.cpp
#(clang-tutor)  count: 3



```
