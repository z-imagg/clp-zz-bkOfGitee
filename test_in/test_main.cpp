#pragma message("VarBE_inserted")
#pragma message("VFIR_inserted")
#define CASE_SAME(x) case x:
#define INT_T int
typedef double DOUBLE_typedef;

struct Point{
    float x;
    double y;
};
class UserEntity{};
class ScoreRelation{};
class MyClass {
public:
    static int ZERO;
public:
    double m_d1;
    int m_n2;

    void ff1() {_VarDeclLs * _vdLs=_init_varLs_inFn("/fridaAnlzAp/clang-var/test_in/test_main.cpp", "MyClass::ff1", 19, 16); /* 初始化函数变量列表, */   destroyVarLs_inFn(_vdLs); /* 销毁函数变量列表: */return; /* voidFnEndInsertRet: */}
    MyClass( ){_VarDeclLs * _vdLs=_init_varLs_inFn("/fridaAnlzAp/clang-var/test_in/test_main.cpp", "MyClass::MyClass", 20, 15); /* 初始化函数变量列表, */
destroyVarLs_inFn(_vdLs); /* 销毁函数变量列表: */return; /* voidFnEndInsertRet: */}

    MyClass(float f1, int n1)
            :m_d1(f1),m_n2(n1)
    {_VarDeclLs * _vdLs=_init_varLs_inFn("/fridaAnlzAp/clang-var/test_in/test_main.cpp", "MyClass::MyClass", 25, 5); /* 初始化函数变量列表, */
        char c3=1+n1;
    destroyVarLs_inFn(_vdLs); /* 销毁函数变量列表: */return; /* voidFnEndInsertRet: */}

    ~MyClass(){_VarDeclLs * _vdLs=_init_varLs_inFn("/fridaAnlzAp/clang-var/test_in/test_main.cpp", "MyClass::~MyClass", 29, 15); /* 初始化函数变量列表, */
        int x,y,z;
        UserEntity userEntity;createVar(_vdLs, "class UserEntity", 1)  /* 创建变量通知,  /fridaAnlzAp/clang-var/test_in/test_main.cpp:31,30 */ ;

        Point point0;createVar(_vdLs, "struct Point", 1)  /* 创建变量通知,  /fridaAnlzAp/clang-var/test_in/test_main.cpp:33,21 */ ;
        auto fn_point = [](const Point& point) {
            if(point.x>point.y)
                return point.x+point.y;
            else
                return 0.1;
        };

        fn_point(point0);
    destroyVarLs_inFn(_vdLs); /* 销毁函数变量列表: */return; /* voidFnEndInsertRet: */}

    void voidDemo(int cnt, short chr){_VarDeclLs * _vdLs=_init_varLs_inFn("/fridaAnlzAp/clang-var/test_in/test_main.cpp", "MyClass::voidDemo", 44, 38); /* 初始化函数变量列表, */
    destroyVarLs_inFn(_vdLs); /* 销毁函数变量列表: */return; /* voidFnEndInsertRet: */}

    void voidDemo2(UserEntity userEntity){_VarDeclLs * _vdLs=_init_varLs_inFn("/fridaAnlzAp/clang-var/test_in/test_main.cpp", "MyClass::voidDemo2", 47, 42); /* 初始化函数变量列表, */
        if(true){
            destroyVarLs_inFn(_vdLs); /* 销毁函数变量列表: */return;
        }
    destroyVarLs_inFn(_vdLs); /* 销毁函数变量列表: */return; /* voidFnEndInsertRet: */}



};

int MyClass::ZERO=0;

void voidDemo3(){_VarDeclLs * _vdLs=_init_varLs_inFn("/fridaAnlzAp/clang-var/test_in/test_main.cpp", "voidDemo3", 59, 17); /* 初始化函数变量列表, */
    int k=0;
    k++;

destroyVarLs_inFn(_vdLs); /* 销毁函数变量列表: */return; /* voidFnEndInsertRet: */}
int main(int argc, char** argv){_VarDeclLs * _vdLs=_init_varLs_inFn("/fridaAnlzAp/clang-var/test_in/test_main.cpp", "main", 64, 32); /* 初始化函数变量列表, */
    MyClass varMyClass;createVar(_vdLs, "class MyClass", 1)  /* 创建变量通知,  /fridaAnlzAp/clang-var/test_in/test_main.cpp:65,23 */ ;
    static Point pnt1;createVar(_vdLs, "struct Point", 1)  /* 创建变量通知,  /fridaAnlzAp/clang-var/test_in/test_main.cpp:66,22 */ ;
    struct Point pnt2;createVar(_vdLs, "struct Point", 1)  /* 创建变量通知,  /fridaAnlzAp/clang-var/test_in/test_main.cpp:67,22 */ ;
    {
        struct Point * ptr1=&pnt1;
        struct Point * ptr2=&pnt2;
        struct Point pnt3;createVar(_vdLs, "struct Point", 1)  /* 创建变量通知,  /fridaAnlzAp/clang-var/test_in/test_main.cpp:71,26 */ ;
    }

    destroyVarLs_inFn(_vdLs); /* 销毁函数变量列表: */return 0;
}