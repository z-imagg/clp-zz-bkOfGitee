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
export CLANG_TUTOR_DIR=/pubx/source_code_rewrite/clang_plugin_demo/clang-tutor/
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


-------------
> 继续换成： docker
> [install docker at ubuntu ](https://docs.docker.com/engine/install/ubuntu/)

```bash
sudo systemctl start docker
sudo docker pull centos:7
sudo docker run --name centos7_clangPlugin_devEnv -itd -v /pubx/:/pubx/ -v /llvm_release_home/:/llvm_release_home/ centos:7
sudo docker exec -it  centos7_clangPlugin_devEnv  bash
```

```bash
export LLVM_HOME=/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/
export  LD_LIBRARY_PATH=$LLVM_HOME/lib:$LLVM_HOME/libexec:$LLVM_HOME/lib/clang/15.0.0/lib/x86_64-unknown-linux-gnu/
export PATH=$LLVM_HOME/bin:$PATH
#报错 glibc2.7找不到
```

------------
> 继续换
> 主机os改为ubuntu20.04
```bash
sudo docker pull kernelci/clang-15:latest
sudo docker  run   --security-opt seccomp=unconfined   --name centos7_clang15Plugin_devEnv -itd -v /pubx/:/pubx/   kernelci/clang-15:latest
sudo docker exec -it  centos7_clang15Plugin_devEnv  bash

#
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
which clang
#/usr/lib/llvm-15/bin/clang


#export Clang_DIR=/usr/lib/llvm-15/
export Clang_DIR=/pubx/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/export CLANG_TUTOR_DIR=/pubx/source_code_rewrite/clang_plugin_demo/clang-tutor/
cd $CLANG_TUTOR_DIR
mkdir build
cd build
cmake -DCT_Clang_INSTALL_DIR=$Clang_DIR $CLANG_TUTOR_DIR/HelloWorld/
make
# Run the plugin
$Clang_DIR/bin/clang -cc1 -load ./libHelloWorld.{so|dylib} -plugin hello-world $CLANG_TUTOR_DIR/test/HelloWorld-basic.cpp
##正常编译
```
