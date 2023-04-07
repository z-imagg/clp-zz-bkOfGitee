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
 
> [install docker at ubuntu ](https://docs.docker.com/engine/install/ubuntu/)

```bash
sudo systemctl start docker
sudo docker pull ubuntu:20.04
sudo docker run --name ubuntu2004_clangPlugin_devEnv -itd -v /pubx/:/pubx/ -v /llvm_release_home/:/llvm_release_home/ ubuntu:20.04
sudo docker exec -it  ubuntu2004_clangPlugin_devEnv  bash
```

  



```bash 
export LLVM_HOME=/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/
export PATH=$LLVM_HOME/bin:$PATH
apt install cmake build-essential
 
export Clang_DIR=/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/
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
