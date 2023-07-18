# 步骤1. opensuse/leap:15.4 命令行下编译

```bash
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
sudo docker pull opensuse/leap:15.4
sudo docker run --name opensuse_leap154_clang15Plugin_devEnv -itd -p 2201:22 -v /pubx/:/pubx/ -v /llvm_release_home/:/llvm_release_home/ opensuse/leap:15.4
sudo docker exec -it  opensuse_leap154_clang15Plugin_devEnv  bash
```
> 假定宿主机ip为: 192.168.71.128,  则 ssh root@192.168.71.128:2201 可以访问 docker实例opensuse_leap154_clang15Plugin_devEnv


> opensuse 清华镜像设置：https://mirrors.tuna.tsinghua.edu.cn/help/opensuse/
```bash
#opensuse 清华镜像设置：https://mirrors.tuna.tsinghua.edu.cn/help/opensuse/
zypper mr -da

zypper ar -cfg 'http://mirrors.tuna.tsinghua.edu.cn/opensuse/distribution/leap/$releasever/repo/oss/' mirror-oss
zypper ar -cfg 'http://mirrors.tuna.tsinghua.edu.cn/opensuse/distribution/leap/$releasever/repo/non-oss/' mirror-non-oss
zypper ar -cfg 'http://mirrors.tuna.tsinghua.edu.cn/opensuse/update/leap/$releasever/oss/' mirror-update
zypper ar -cfg 'http://mirrors.tuna.tsinghua.edu.cn/opensuse/update/leap/$releasever/non-oss/' mirror-update-non-oss
```

```bash

zypper install  -y tar gzip
#clion2022.3.3 remote development需要用到gzip命令

zypper info gdb  #Version        : 11.1-8.30.1
#CLion-2021.2.2.exe: 支持的gdb版本范围：7.8.x-10.2.x

zypper install  -y gdb
```

```bash
zypper install  -y openssh-server

ssh-keygen -A
#ssh-keygen: generating new host keys: RSA DSA ECDSA ED25519

grep -E 'PermitRootLogin\s+yes'  /etc/ssh/sshd_config  || echo """PermitRootLogin yes""" >>  /etc/ssh/sshd_config

/usr/sbin/sshd  -f /etc/ssh/sshd_config
#如何重启sshd: 先kill, 再启动

curl localhost:22

passwd root #修改root密码
```

```bash
#宿主机上尝试连接docker容器
ssh root@localhost -p 2201
```

```bash
zypper info  cmake     #版本: 3.20.4-150400.2.5
zypper install  -y cmake
zypper install -y  binutils-gold
zypper install -y libstdc++6 libstdc++-devel
```

```bash

zypper install -y gcc 
zypper remove -y gcc #并不需要gcc，只是需要gcc的依赖库
```

```bash
ldconfig -p | grep curse
#        libncursesw.so.6 (libc6,x86-64) => /lib64/libncursesw.so.6
#        libncurses.so.6 (libc6,x86-64) => /lib64/libncurses.so.6
#        libncurses++w.so.6 (libc6,x86-64) => /usr/lib64/libncurses++w.so.6
#        libncurses++.so.6 (libc6,x86-64) => /usr/lib64/libncurses++.so.6

```


```bash
cd /pubx/source_code_rewrite/clang_plugin_demo/clang-tutor/
rm -fr build; mkdir build ; cd build
cmake -DCT_Clang_INSTALL_DIR=/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/  -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -DCURSES_LIBRARY=/lib64/libncurses.so.6 -DCURSES_INCLUDE_PATH=/usr/include/   -DCMAKE_EXPORT_COMPILE_COMMANDS=True   -DCMAKE_C_COMPILER=/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/bin/clang -DCMAKE_CXX_COMPILER=/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/bin/clang++    -DLLVM_DIR=/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4 ..

#或:
#cmake -DCMAKE_TOOLCHAIN_FILE=../toolchain.cmake ..

make
#正常编译

#编译出的插件如下：
find `pwd` -name "*.so"
#/pubx/source_code_rewrite/clang_plugin_demo/clang-tutor/build/lib/libLACommenter.so
#/pubx/source_code_rewrite/clang_plugin_demo/clang-tutor/build/lib/libCodeStyleChecker.so
#/pubx/source_code_rewrite/clang_plugin_demo/clang-tutor/build/lib/libObfuscator.so
#/pubx/source_code_rewrite/clang_plugin_demo/clang-tutor/build/lib/libUnusedForLoopVar.so
#/pubx/source_code_rewrite/clang_plugin_demo/clang-tutor/build/lib/libHelloWorld.so
#/pubx/source_code_rewrite/clang_plugin_demo/clang-tutor/build/lib/libCodeRefactor.so

#运行hello world插件
/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/bin/clang -cc1 -load /pubx/source_code_rewrite/clang_plugin_demo/clang-tutor/build/lib/libHelloWorld.so -plugin hello-world /pubx/source_code_rewrite/clang_plugin_demo/clang-tutor/test/HelloWorld-basic.cpp
#(clang-tutor)  file: /pubx/source_code_rewrite/clang_plugin_demo/clang-tutor/test/HelloWorld-basic.cpp
#(clang-tutor)  count: 3

#运行CodeRefactor插件
/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/bin/clang -cc1 -load   /pubx/clang-tutor/cmake-build-debug/lib/libCodeRefactor.so -plugin CodeRefactor /pubx/clang-tutor/test/CodeRefactor_Class.cpp

#独立运行CodeRefactor
/pubx/clang-tutor/cmake-build-debug/bin/ct-code-refactor /pubx/clang-tutor/test/CodeRefactor_Class.cpp
#虽然报错“Error while trying to load a compilation database:” ,但正常做了CodeRefactor

#运行CodeStyleChecker插件
/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/bin/clang -cc1 -load   /pubx/clang-tutor/cmake-build-debug/lib/libCodeStyleChecker.so -plugin CSC /pubx/clang-tutor/test/CodeStyleCheckerVector.cpp

#独立运行CodeStyleChecker
/pubx/clang-tutor/cmake-build-debug/bin/ct-code-style-checker /pubx/clang-tutor/test/CodeStyleCheckerVector.cpp
#不知道为何，只是报错“Error while trying to load a compilation database:” 且没做事情
```


# 步骤2. clion 2022.3.3 Remote Development (远程开发) 基于  "步骤1. opensuse/leap:15.4 命令行下编译"
## 2.1. 安装clion 2022.3.3, 


## 2.2. crack 
> crack参照：[jetbrains_crack](https://gitcode.net/pubx/jetbrains/jetbrains_crack)

## 2.3 准备
1. 下载CLion-2022.3.3.tar.gz: 
2. git clone待开发项目仓库 clang_plugin_demo/clang-tutor

## 2.3. clion 2022.3.3 Remote Development (远程开发)
> 打开 win10上刚安装好的 clion2022.3.3
1. "File" --> 
2. "Remote Development" --> "ssh", "New Connection" --> 192.168.71.128:2201,root,root --> 
3. "Check Connection and Continue" -->  "Installation options" :"upload installer file"  -->
4. "Select .tar.gz file" : "C:/Users/zzz/Downloads/CLion-2022.3.3.tar.gz"（win10中刚刚下载的CLion-2022.3.3.tar.gz） -->
5. "Project directory" : "/pubx/source_code_rewrite/clang_plugin_demo/clang-tutor" --> "Upload IDE and Connect"

>详见下图
![](https://gitcode.net/pubx/source_code_rewrite/clang_plugin_demo/clang-tutor/-/raw/main/doc/clion2022.3.3-remote-development.png)

## 2.4 构建
![](https://gitcode.net/pubx/source_code_rewrite/clang_plugin_demo/clang-tutor/-/raw/main/doc/compile_helloworld.png)

## 2.5 调试
```bash
#相当于:
cd /pubx/source_code_rewrite/clang_plugin_demo/clang-tutor/cmake-build-debug
/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/bin/clang  -cc1 -load cmake-build-debug/lib/libHelloWorld.so -plugin hello-world test/HelloWorld-basic.cpp
```
![](https://gitcode.net/pubx/source_code_rewrite/clang_plugin_demo/clang-tutor/-/raw/main/doc/debug_helloworld.png)

