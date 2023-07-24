
namespace myNs1{
namespace myNs2{

constexpr int func0(){
  return 90;//constexpr func0
}

struct User{
constexpr int func1(){
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
	char ch,sex='m';//命名空间内的 函数 funOutImpl: 在结构体定义外实现函数体
  ch++;
  while(ch!='a'){
    int num,sum=0;
    char flag;
    float delta=0.9, alpha, ZERO=0.0, PI=3.14, DIFF;
    ch--;
    //此块末尾 应该释放8个变量, 而不是3个变量。
  }
  return ch;
}


char topOutFunc(float f1, double d2){
  int arr[]={8,0,-1,99};
  for(int x: arr){
    bool ok=x%2==0;
    if(ok) break;
  }

  {
    double sum;
    {
      short fix;
    }
  }
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
  int k;
	char c=f1>0 && d2<0?'a':'b';//无命名空间 的 顶层函数实现.
  return c;
}

#include <exception>
#include <iostream>

static void* staticFunc(int* arg){
  char (*funcPtr)(int, char, short)=0;
  try{
    int num=*arg;
    char ch='A';
    short shrt=(short)num/10;
    try{
      char result=funcPtr(num,ch,shrt);
      if(!result)
        throw "err02";
    }catch (std::exception& exception){
      std::cout<< exception.what() << std::endl;
    }
  }catch (const char* errmsg){
    std::cout<< errmsg << std::endl;
  }catch(std::exception & e){
    std::cout<< e.what() << std::endl;
  }catch (...){
    throw "other exception";
  }
}
