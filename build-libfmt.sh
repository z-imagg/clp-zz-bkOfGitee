cd /pubx/

REPO_HOME=/pubx/fmt/
BUILD_HOME=$REPO_HOME/build

#https://github.com/fmtlib/fmt.git
git clone git@gitcode.net:pubx/fmtlib/fmt.git
cd $REPO_HOME
git checkout 10.0.0


rm -fr $BUILD_HOME
mkdir $BUILD_HOME && cd $BUILD_HOME
cmake $REPO_HOME
make -j8

ls -lh $BUILD_HOME/libfmt.a
