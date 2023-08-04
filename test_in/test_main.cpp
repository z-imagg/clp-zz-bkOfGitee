#define MY_VAR_DECL_STMT(N) double sum_##N = 0;
// ## 连接 sum 和 N
#define  MY_VAR_PLUS_STMT(N)  sum_##N += 7     ;

class MyClass {
public:
    static int ZERO;
public:
    int myFunction(int a, int b);
};
int MyClass::ZERO=0.001;
int MyClass::myFunction(int a, int b) {
  float fff = a + b / 10  + MyClass::ZERO     ;  char chr   = ( char ) ( 0.9 * a / b + fff * 100 )   ;
  MY_VAR_DECL_STMT(1)
  int x,y,z;
  MY_VAR_PLUS_STMT(1)
  MY_VAR_DECL_STMT(2)
  return a + b;
}