#include <iostream>
/**
 * 这是一个示例类
 */
/**__BrcOkFlagText*/class MyClass {
public:
    static int ZERO;
public:
    /**
     * 这是一个示例成员函数
     * @param a 参数a
     * @param b 参数b
     * @return 返回结果
     */
    int myFunction(int a, int b) {
      return a + b;
    }

  char* switchDemo(int cnt, short chr){
    double sum=1.*cnt*chr;
    start: char L;
    switch(chr){
      case 0:
      {
        L=(cnt+chr)%50;
        return &L+1;
      }


      case 1:{
        int unit;
        unit=18;
        return NULL;
      }/*TraverseSwitchStmt*/case 2:{
        cnt=L*L;
        cnt++;
        break;
      }/*TraverseSwitchStmt*/case 3:{
        return &L;
      }/*TraverseSwitchStmt*/case 4:
      {
        L++;
        sum=L+sum;
        if (L)  {goto start;}/*TraverseIfStmt:thenStmt*/
      }
      return (char*)0;
      case 6:{
        int zB;
        break;
      }
      ////////
      case 7:{
        int uuuuu;
        break;
      }
      default:{
        sum+=chr*cnt;
        return &L;
    }/*TraverseSwitchStmt*/}
    return NULL;
  }
};

int MyClass::ZERO=0;

// 这是一个示例全局函数
/**
 * @brief 这是一个示例全局函数
 * @param x 参数x
 * @param y 参数y
 * @return 返回结果
 */
int myGlobalFunction(int x, int y) {
  char ch;
  if(x>y)
    if(x<5)
      if (y>600)
      while(x++ + y > 12)
        {y += x  * 2  - y  + MyClass::ZERO    ;}/*TraverseWhileStmt*/
      else if(y>0)
        for(int t=0; t<x+y; t+=x)
          {y = x + t - 1/t ;}/*TraverseForStmt*/
      else
        {return ch * x / y  ;}/*TraverseIfStmt:elseStmt*/
    else
      {ch = ch * myGlobalFunction(ch, y)   ;}/*TraverseIfStmt:elseStmt*/

  return x * y;
}

void func_macro_inc(){
  double PI;
  if(true)
#include "Math.def.h"
}
int main() {
  MyClass obj;
  int result = obj.myFunction(10, 20);
  int x,y,z;
  if(true)
  {x = y + MyClass::ZERO   ;}/*TraverseIfStmt:thenStmt*/  
  else
  {z *= x /  myGlobalFunction(z, y)   ;}/*TraverseIfStmt:elseStmt*/    

  int product = myGlobalFunction(5, 6);
  std::cout << "Product: " << product << std::endl;

  return 0;
}