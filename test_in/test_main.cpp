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

    void ff1() { return; }
    MyClass( ){
        return; /* voidFnEndInsertRet: */}

    //构造函数MyClass中的return和左花括号  若 不在同一行，则正常（为init在前 destroy在后）
    //                                 若 在同一行，  则错误（为destroy在前 init在后）
    //不过这解释不了 为何函数ff1 即使 return和做花括号 在同一行 ，也正常（为init在前 destroy在后）
    //不过 由此 可得到建议， clang插件 修改源码 应该避免对同一行做多个Insert动作，若非要做 则事先确保多个Insert修改处不是同一行， 否则插入顺序可能是非预期的。
    //【因此得解决办法】 插件voidFnEndInsertRet 插入内容增加换行 即 'return;'改为'\nreturn;'


};

int MyClass::ZERO=0;