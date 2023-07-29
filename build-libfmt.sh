cd /pubx/

REPO_HOME=/pubx/fmt/
BUILD_HOME=$REPO_HOME/build

#https://github.com/fmtlib/fmt.git
git clone git@gitcode.net:pubx/fmtlib/fmt.git
cd $REPO_HOME
git checkout 10.0.0

_CXX__FLAG=" -fno-omit-frame-pointer -Wall   -O0  -fPIC "


rm -fr $BUILD_HOME && mkdir $BUILD_HOME && cd $BUILD_HOME
cmake \
-DFMT_TEST=OFF \
-DCMAKE_CXX_FLAGS="$_CXX__FLAG"  -DCMAKE_C_FLAGS="$_CXX__FLAG" \
-S $REPO_HOME

make -j8

ls -lh $BUILD_HOME/libfmt.a
