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
sudo docker run --name opensuse_leap154_clangPlugin_devEnv -itd -p 2204:22 -v /pubx/:/pubx/ -v /llvm_release_home/:/llvm_release_home/ opensuse/leap:15.4
sudo docker exec -it  opensuse_leap154_clangPlugin_devEnv  bash
```


```bash
#opensuse 清华镜像设置：https://mirrors.tuna.tsinghua.edu.cn/help/opensuse/
zypper info  cmake     #版本: 3.20.4-150400.2.5
zypper install   cmake
zypper install binutils-gold
zypper install gcc
zypper install libstdc++6 libstdc++-devel
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
```
