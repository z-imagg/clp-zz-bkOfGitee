BUILD_HOME=/pubx/clang-ctk/build/
mkdir -p $BUILD_HOME
rm -fr $BUILD_HOME/*
cd $BUILD_HOME
# LIB_CTk_CXX_FLAGS="  "
# -rwxrwxr-x 1 zz zz 874K  7月 24 13:13 /pubx/clang-ctk/build/lib/libCTk.so
#构建带调试信息的Release版本, 当出现问题方便排查:
LIB_CTk_CXX_FLAGS=" -fno-omit-frame-pointer -Wall   -O0    -g1"
# -rwxrwxr-x 1 zz zz 4.6M  7月 24 14:50 lib/libCTk.so
# LIB_CTk_CXX_FLAGS=" -fno-omit-frame-pointer -Wall   -O0    -g2"
# -rwxrwxr-x 1 zz zz 27M  7月 24 15:03 /pubx/clang-ctk/build/lib/libCTk.so
cmake -DCMAKE_BUILD_TYPE=Release   -DCMAKE_CXX_FLAGS="$LIB_CTk_CXX_FLAGS"   -DCMAKE_C_FLAGS="$LIB_CTk_CXX_FLAGS"  ..
#-DCMAKE_VERBOSE_MAKEFILE=True
make -j8
ls -lh $BUILD_HOME/lib/lib*.so