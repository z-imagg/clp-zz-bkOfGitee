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

zypper info gdb  #Version        : 11.1-8.30.1
#CLion-2021.2.2.exe: 支持的gdb版本范围：7.8.x-10.2.x

zypper install  -y gdb
```

```bash
zypper install  -y openssh-server
ssh-keygen -A
echo """PermitRootLogin yes""" >>  /etc/ssh/sshd_config
/usr/sbin/sshd  -f /etc/ssh/sshd_config
#如何重启sshd: 先kill, 再启动
```

```bash
zypper info  cmake     #版本: 3.20.4-150400.2.5
zypper install  -y cmake
zypper install -y  binutils-gold
zypper install -y gcc
zypper install -y libstdc++6 libstdc++-devel
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
mkdir build
cd build
cmake -DCT_Clang_INSTALL_DIR=/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/  -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -DCURSES_LIBRARY=/lib64/libncurses.so.6 -DCURSES_INCLUDE_PATH=/usr/include/   -DCMAKE_EXPORT_COMPILE_COMMANDS=True   -DCMAKE_C_COMPILER=/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/bin/clang -DCMAKE_CXX_COMPILER=/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4/bin/clang++    -DLLVM_DIR=/llvm_release_home/clang+llvm-15.0.0-x86_64-linux-gnu-rhel-8.4 ..

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

```


# 步骤2. clion 2022.3.3 Remote Development (远程开发) 基于  "步骤1. opensuse/leap:15.4 命令行下编译"
