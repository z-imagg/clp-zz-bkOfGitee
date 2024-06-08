
#ifndef MyClz_H
#define MyClz_H

#define CASE_SAME(x) case x:
#define INT_T int
typedef double DOUBLE_typedef;

struct Point{
    float x;
    double y;
};

namespace my_nsp {

    class UserEntity{};
    class ScoreRelation{};

class MyClass {
public:
    static int ZERO;
public:
    double m_d1;
    int m_n2;

    void ff1() {   }
    MyClass( ){}

    MyClass(float f1, int n1)
            :m_d1(f1),m_n2(n1)
    {
        char c3=1+n1;
    }

    ~MyClass(){
        int x,y,z;
        UserEntity userEntity;

        Point point0;
        auto fn_point = [](const Point& point) {
            if(point.x>point.y)
                return point.x+point.y;
            else
                return 0.1;
        };

        fn_point(point0);
    }

    void voidDemo(int cnt, short chr){
    }

    void voidDemo2(UserEntity userEntity){
        if(true){
            return;
        }
    }



};

} // my_nsp

#endif //MyClz_H
