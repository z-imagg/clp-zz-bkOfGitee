#include "t_clock_tick.h"

#define LLVM_FALLTHROUGH [[gnu::fallthrough]]

char topOutFunc000(float f1, double d2){
  X__t_clock_tick(/*栈生*/1, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
int diff;
  X__t_clock_tick(/*栈生*/1, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
int arr[]={8,0,-1,99};
  X__t_clock_tick(/*栈生*/0, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
for(int p:arr)
  for(int j:arr){
    X__t_clock_tick(/*栈生*/1, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
int zz;
  X__t_clock_tick(/*栈生*/0, /*栈死*/1, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
}
  X__t_clock_tick(/*栈生*/0, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
if(true)
  for(int q: arr)
//  for(int q=0; q< 8 ; q++)
    if (q>0)//if1
      if(diff>9)//if2
        diff+=q;
      else//else1
      if (q+diff>q*diff)//if3
        arr[q]++;
      else//else2
      if(false){//if4
        X__t_clock_tick(/*栈生*/0, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
arr[0]-=diff;
      }else//else3
        arr[diff]*=7;


X__t_clock_tick(/*栈生*/0, /*栈死*/2, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
}



namespace myNs1{
    namespace myNs2{


        constexpr int func0(int z){
          int a[]={};
          for(int i:a)
            i=z;
          return 90;//constexpr func0
        }


        class Product{
        public:
            Product(){

            }
            Product(int id, bool ok, char cate='Z')
                    :id(id),ok(ok),cate(cate),owner_id(-1)
            {
              X__t_clock_tick(/*栈生*/2, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
int thanks,memory=100;
              X__t_clock_tick(/*栈生*/0, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
while(true){
                X__t_clock_tick(/*栈生*/1, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
bool err=false;
                X__t_clock_tick(/*栈生*/0, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
if(err){
                  X__t_clock_tick(/*栈生*/0, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
break;
                }
              X__t_clock_tick(/*栈生*/0, /*栈死*/1, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
}
            X__t_clock_tick(/*栈生*/0, /*栈死*/2, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
}
            Product(Product &&Other) = default;
            Product &operator=(Product &&Other) = default;

            void switch_demo_func(){
              X__t_clock_tick(/*栈生*/1, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
int z;

              X__t_clock_tick(/*栈生*/0, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
switch(z)
              {//switch身体开始

                case 1://case 1
                {
                  X__t_clock_tick(/*栈生*/1, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
int idea; X__t_clock_tick(/*栈生*/3, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
float aa,bb,cc=0.9; X__t_clock_tick(/*栈生*/1, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
double sex;
                  X__t_clock_tick(/*栈生*/0, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
z+X__t_clock_tick(/*栈生*/0, /*栈死*/5, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
+;//这里有个bug, 可能是所用clang15版本的bug, 组合语句的子节点textVec[3] 本来是 z++, 但这里得到是z. 结合前面的插入释放语句FALLTHROUGH到块倒数第二条语句后，这里显然发生了错误。
                  LLVM_FALLTHROUGH;
                }

                case 2:
                {//case 2 身体 开始
                  X__t_clock_tick(/*栈生*/1, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
bool valid;
                  X__t_clock_tick(/*栈生*/0, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
if(valid){
                    X__t_clock_tick(/*栈生*/0, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
break;
                  }else{
                    X__t_clock_tick(/*栈生*/0, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
valid=true;
                  }
                X__t_clock_tick(/*栈生*/0, /*栈死*/1, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
}//case 2 身体 结尾

                default:
                {//switch的default 身体 开始
                  float f;//switch's default content
                  return;
                }//witch的default 身体结尾

              }//switch(z) 结尾

            X__t_clock_tick(/*栈生*/0, /*栈死*/1, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
}//switch_demo_func函数 结尾

        private:
            int id;
            bool ok;
            char cate;
            int owner_id;
        };

        struct User{
            constexpr int func1(){
              return 1;//func1: 在结构体定义处实现函数体
            }
            int func2(){
              X__t_clock_tick(/*栈生*/0, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
return  func1();//func2: 在结构体定义处实现函数体
            }

            int funOutImpl();
        };

    }

}

int myNs1::myNs2::User::funOutImpl() {
  X__t_clock_tick(/*栈生*/2, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
char ch,sex='m';//命名空间内的 函数 funOutImpl: 在结构体定义外实现函数体
  X__t_clock_tick(/*栈生*/0, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
ch++;
  X__t_clock_tick(/*栈生*/0, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
while(ch!='a'){
    X__t_clock_tick(/*栈生*/2, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
int num,sum=0;
    X__t_clock_tick(/*栈生*/1, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
char flag;
    X__t_clock_tick(/*栈生*/5, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
float delta=0.9, alpha, ZERO=0.0, PI=3.14, DIFF;
    X__t_clock_tick(/*栈生*/0, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
ch--;
    //此块末尾 应该释放8个变量, 而不是3个变量。
  X__t_clock_tick(/*栈生*/0, /*栈死*/8, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
}
  X__t_clock_tick(/*栈生*/0, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
X__t_clock_tick(/*栈生*/0, /*栈死*/2, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
return ch;
}


char topOutFunc(float f1, double d2){
  X__t_clock_tick(/*栈生*/1, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
int arr[]={8,0,-1,99};
  X__t_clock_tick(/*栈生*/0, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
for(int x: arr){
    X__t_clock_tick(/*栈生*/1, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
bool ok=x%2==0;
    X__t_clock_tick(/*栈生*/0, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
if(ok) break;
  X__t_clock_tick(/*栈生*/0, /*栈死*/1, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
}

  X__t_clock_tick(/*栈生*/0, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
{
    X__t_clock_tick(/*栈生*/1, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
double sum;
    X__t_clock_tick(/*栈生*/0, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
{
      X__t_clock_tick(/*栈生*/1, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
short fix;
    X__t_clock_tick(/*栈生*/0, /*栈死*/1, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
}
  X__t_clock_tick(/*栈生*/0, /*栈死*/1, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
}
  X__t_clock_tick(/*栈生*/0, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
if(f1<d2){
    X__t_clock_tick(/*栈生*/1, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
int age;//CompoundStmt举例: 第1层CompoundStmt
    X__t_clock_tick(/*栈生*/1, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
float xx;
    X__t_clock_tick(/*栈生*/0, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
xx=age*10;
    X__t_clock_tick(/*栈生*/0, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
{
      X__t_clock_tick(/*栈生*/1, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
char tv;//CompoundStmt举例: 第2层CompoundStmt
      X__t_clock_tick(/*栈生*/0, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
if(tv=='b'){
        X__t_clock_tick(/*栈生*/0, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
tv*=4;
        X__t_clock_tick(/*栈生*/0, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
return tv+4;//CompoundStmt举例: 第3层CompoundStmt
      }
    X__t_clock_tick(/*栈生*/0, /*栈死*/1, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
}
    X__t_clock_tick(/*栈生*/0, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
X__t_clock_tick(/*栈生*/0, /*栈死*/2, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
return 'x';
  }
  X__t_clock_tick(/*栈生*/1, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
int k;
  X__t_clock_tick(/*栈生*/1, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
char c=f1>0 && d2<0?'a':'b';//无命名空间 的 顶层函数实现.
  X__t_clock_tick(/*栈生*/0, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
X__t_clock_tick(/*栈生*/0, /*栈死*/3, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
return c;
}

#include <exception>
#include <iostream>

static void* staticFunc(int* arg){
  X__t_clock_tick(/*栈生*/1, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
char (*funcPtr)(int, char, short)=0;
  X__t_clock_tick(/*栈生*/0, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
try{
    X__t_clock_tick(/*栈生*/1, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
int num=*arg;
    X__t_clock_tick(/*栈生*/1, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
char ch='A';
    X__t_clock_tick(/*栈生*/1, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
short shrt=(short)num/10;
    X__t_clock_tick(/*栈生*/0, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
try{
      X__t_clock_tick(/*栈生*/1, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
char result=funcPtr(num,ch,shrt);
      X__t_clock_tick(/*栈生*/0, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
if(!result)
        throw "err02";
    X__t_clock_tick(/*栈生*/0, /*栈死*/1, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
}catch (std::exception& exception){
      std::cout<< exception.what() << std::endl;
    }
  X__t_clock_tick(/*栈生*/0, /*栈死*/3, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
}catch (const char* errmsg){
    std::cout<< errmsg << std::endl;
  }catch(std::exception & e){
    std::cout<< e.what() << std::endl;
  }catch (...){
    throw "other exception";
  }
X__t_clock_tick(/*栈生*/0, /*栈死*/1, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
}
