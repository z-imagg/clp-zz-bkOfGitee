#include "rntm_c__TmPnt_ThrLcl.h"

#include <iostream>

class TestClass{
public:
    static void func1(int tmPnt_new);
};

void TestClass::func1(int tmPnt_new){
TL_TmPnt__update(tmPnt_new);
}

int main(int argc, char** argv){
TestClass::func1(40);
int tmPnt=TL_TmPnt__get();
std::cout<<"tmPnt="<<tmPnt<<std::endl;
}