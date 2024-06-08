#include "MyClz.h"



int main(int argc, char** argv){
    my_nsp::MyClass varMyClass;
    static Point pnt1;
    struct Point pnt2;
    {
        struct Point * ptr1=&pnt1;
        struct Point * ptr2=&pnt2;
        struct Point pnt3;
    }

    return 0;
}

