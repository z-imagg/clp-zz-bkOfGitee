#include "t_clock_tick.h"

namespace myNs1{
namespace myNs2{

struct User{
constexpr int func1(){
X__t_clock_tick(0, 0, 0, 0);
return 1;//func1: 在结构体定义处实现函数体
}
int func2(){
return  func1();//func2: 在结构体定义处实现函数体
}

int funOutImpl();
};

}

}

int myNs1::myNs2::User::funOutImpl() {
	X__t_clock_tick(0, 0, 0, 0);
char ch,sex='m';//命名空间内的 函数 funOutImpl: 在结构体定义外实现函数体
  ch++;
  X__t_clock_tick(0, 0, 0, 0);
X__t_clock_tick(0, 1, 0, 0);
return ch;
}


char topOutFunc(float f1, double d2){
  {//第1层组合语句块 插入时钟 结果正确
    X__t_clock_tick(0, 0, 0, 0);
double sum;
    {//第2层次组合语句块 插入时钟 结果正确
      X__t_clock_tick(0, 0, 0, 0);
short fix;
    X__t_clock_tick(0, 1, 0, 0);
}
  X__t_clock_tick(0, 1, 0, 0);
}
  X__t_clock_tick(0, 0, 0, 0);
if(f1<d2){
    int age;//CompoundStmt举例: 第1层CompoundStmt
    float xx;
    xx=age*10;
    {
      char tv;//CompoundStmt举例: 第2层CompoundStmt
      if(tv=='b'){
        tv*=4;
        return tv+4;//CompoundStmt举例: 第3层CompoundStmt
      }
    }
    return 'x';
  }
  X__t_clock_tick(0, 0, 0, 0);
int k;
	X__t_clock_tick(0, 0, 0, 0);
char c=f1>0 && d2<0?'a':'b';//无命名空间 的 顶层函数实现.
  X__t_clock_tick(0, 0, 0, 0);
X__t_clock_tick(0, 2, 0, 0);
return c;
}
