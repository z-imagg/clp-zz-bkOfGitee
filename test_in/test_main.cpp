#pragma message("VarBE_inserted")
#include "MyClz.h"



int main(int argc, char** argv){_VarDeclLs * _vdLs=_init_varLs_inFn__RtCxx("/fridaAnlzAp/clp-zz/test_in/test_main.cpp", "main", 5, 32); /* 初始化函数变量列表, */
    my_nsp::MyClass varMyClass;createVar__RtCxx(_vdLs, "my_nsp::MyClass", 1)  /* 创建变量通知,  /fridaAnlzAp/clp-zz/test_in/test_main.cpp:6,31 */ ;
    static Point pnt1;createVar__RtCxx(_vdLs, "struct Point", 1)  /* 创建变量通知,  /fridaAnlzAp/clp-zz/test_in/test_main.cpp:7,22 */ ;
    struct Point pnt2;createVar__RtCxx(_vdLs, "struct Point", 1)  /* 创建变量通知,  /fridaAnlzAp/clp-zz/test_in/test_main.cpp:8,22 */ ;
    {
        struct Point * ptr1=&pnt1;
        struct Point * ptr2=&pnt2;
        struct Point pnt3;createVar__RtCxx(_vdLs, "struct Point", 1)  /* 创建变量通知,  /fridaAnlzAp/clp-zz/test_in/test_main.cpp:12,26 */ ;
    }

    destroyVarLs_inFn__RtCxx(_vdLs); /* 销毁函数变量列表: */return 0;
}