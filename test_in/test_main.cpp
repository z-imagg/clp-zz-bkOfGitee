#include <stdio.h>
class MyClass {
public:
    static int ZERO;
public:

  char* switchDemo(int cnt, short chr){
    double sum=1.*cnt*chr;
    float PI;
    start: char L;
    switch(chr){
      case 0:
      int FLASH;//case0内定义的变量被case4使用了，因此case0不能用花括号包裹。
      for(int z=0; z<11; z++)
        PI*=2;
      L=(cnt+chr)%50;
#include "Math.def.h"
      return &L+1;//case0 结束位置左移1

      case 1:
#define IS_ALICE true
        int/*此非case1结束*/ unit;//case1共3行单语句, 这里是case1的第1行单语句
        unit=18;
        return 0;//case1 结束位置
      case 2:
        cnt/*此非case2结束*/=L*L;
        cnt++;
        break;//case2 结束
      case 3:
        return/*此非case3结束*/ &L;//case3 结束
      case 4:
      {
      }
      double doubleMe;//doubleMe的parent0NodeKind:CompoundStmt， doubleMe的parent0的parent0是switch,
      // 很明显了：
      // 直接在'case'下声明的变量属于switch，不属于任何case。而直接在'case{'下声明的变量才属于case
      PI=3.14;
      return (char*)0;
      case 5:{
        int zB;
        break;//case5 结束位置左移1
      }//case5语句结束, 但后面还有一行注释
      ////////
      case 6:{
        int uuuuu;
        break;//case6 结束位置左移1
      }//case6 结束
      default:
        sum/*此非default结束*/+=chr*cnt;
        return &L;//default结束
    }
    return 0;
  }
};

int MyClass::ZERO=0;

