#include <stdio.h>

int (*fun1_sum)(int a, int b);
class MyClass {
public:
    static int ZERO;
public:
    float func_demo(char sharpKind,float* r_ptr, double& area){

      int (*func_p1)(int a, int b)=fun1_sum;

      double PI=3.14;
      double LEN=0;
      if(sharpKind=='s'){
        LEN=2*PI*(*r_ptr);
      }else if(sharpKind=='m'){
        LEN=4*(*r_ptr);
      }
      int (*func_p2)(int a, int b)=fun1_sum;

      for(int i =0; i < *r_ptr; i++){
        area=LEN-PI;
      }


      return LEN;
    }
};

int MyClass::ZERO=0;

