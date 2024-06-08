#include "MyClz.h"
#include <algorithm>
#include <list>


int main(int argc, char** argv){
    my_nsp::MyClass varMyClass;
    static Point pnt1;
    struct Point pnt2;
    {
        struct Point * ptr1=&pnt1;
        struct Point * ptr2=&pnt2;
        struct Point pnt3;
    }


    std::list<my_nsp::MyClass*> ls;

    std::for_each(ls.begin(), ls.end(), [](auto k){ return k; });

    return 0;
}

