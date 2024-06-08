#pragma message("VarBE_inserted")
#include "MyClz.h"
#include <algorithm>
#include <list>


int main(int argc, char** argv){_VarDeclLs * _vdLs=_init_varLs_inFn__RtCxx("/fridaAnlzAp/clp-zz/test_in/test_main.cpp", "main", 6, 32); /* 初始化函数变量列表, */
    my_nsp::MyClass varMyClass;createVar__RtCxx(_vdLs, "my_nsp::MyClass", 1)  /* 创建变量通知,  /fridaAnlzAp/clp-zz/test_in/test_main.cpp:7,31 */ ;
    static Point pnt1;createVar__RtCxx(_vdLs, "struct Point", 1)  /* 创建变量通知,  /fridaAnlzAp/clp-zz/test_in/test_main.cpp:8,22 */ ;
    struct Point pnt2;createVar__RtCxx(_vdLs, "struct Point", 1)  /* 创建变量通知,  /fridaAnlzAp/clp-zz/test_in/test_main.cpp:9,22 */ ;
    {
        struct Point * ptr1=&pnt1;
        struct Point * ptr2=&pnt2;
        struct Point pnt3;createVar__RtCxx(_vdLs, "struct Point", 1)  /* 创建变量通知,  /fridaAnlzAp/clp-zz/test_in/test_main.cpp:13,26 */ ;
    }


    std::list<my_nsp::MyClass*> ls;createVar__RtCxx(_vdLs, "std::list<my_nsp::MyClass *>", 1)  /* 创建变量通知,  /fridaAnlzAp/clp-zz/test_in/test_main.cpp:17,35 */ ;

  std::for_each(ls.begin(), ls.end(), [](my_nsp::MyClass* k){ my_nsp::MyClass* idx=k; destroyVarLs_inFn__RtCxx(_vdLs); /* 销毁函数变量列表: */return k; });

    destroyVarLs_inFn__RtCxx(_vdLs); /* 销毁函数变量列表: */return 0;
}

