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
#centos7的cmake版本为2.x，版本太低，不符合这个项目要求的cmake版本
#centos8 清华镜像源; 并使用此纠正清华镜像centos8路径问题： https://blog.csdn.net/SuvanCode/article/details/123334021
yum info cmake 
yum install cmake gcc

export LLVM_HOME=/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/
export PATH=$LLVM_HOME/bin/:$PATH

#centos8 报错： /usr/bin/ld: cannot find -lstdc++
```
  

```bash
#apt设置清华镜像源
# NO_PUBKEY 871920D1991BC93C 报错，解决： https://www.ljy2345.com/2022/08/docker-run-ubuntu-22-04-appear-no_pubkey-871920d1991bc93c/
# docker启动时加参数 --security-opt seccomp=unconfined  

apt --allow-unauthenticated update
apt-key adv --keyserver keyserver.ubuntu.com --recv-keys 3B4FE6ACC0B21F32
apt-key adv --keyserver keyserver.ubuntu.com --recv-keys 871920D1991BC93C
apt install cmake
```

```bash 
 
export Clang_DIR=/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/
export CLANG_TUTOR_DIR=/pubx/source_code_rewrite/clang_plugin_demo/clang-tutor/
cd $CLANG_TUTOR_DIR
mkdir build
cd build
cmake -DCT_Clang_INSTALL_DIR=$Clang_DIR $CLANG_TUTOR_DIR/HelloWorld/
make
# Run the plugin
$Clang_DIR/bin/clang -cc1 -load ./libHelloWorld.{so|dylib} -plugin hello-world $CLANG_TUTOR_DIR/test/HelloWorld-basic.cpp
##正常编译
```
