

char topOutFunc000(float f1, double d2){
  int diff;
  int arr[]={8,0,-1,99};
  for(int p:arr)
  for(int j:arr){
    int zz;
  }
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
        arr[0]-=diff;
      }else//else3
        arr[diff]*=7;


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
              int thanks,memory=100;
              while(true){
                bool err=false;
                if(err){
                  break;
                }
              }
            }
            Product(Product &&Other) = default;
            Product &operator=(Product &&Other) = default;

            void switch_demo_func(){
              int z;

              switch(z)
              {//switch身体开始

                case 1://case 1
                  break;

                case 2:
                {//case 2 身体 开始
                  bool valid;
                  if(valid){
                    break;
                  }else{
                    valid=true;
                  }
                  [[gnu::fallthrough]];//在 TraverseCompoundStmt 即 组合语句处理 中， 若发现当前语句 是 [[gnu::fallthrough]] ， 则下一条语句前不要插入。
                }//case 2 身体 结尾

                default:
                {//switch的default 身体 开始
                  float f;//switch's default content
                  return;
                }//witch的default 身体结尾

              }//switch(z) 结尾

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
