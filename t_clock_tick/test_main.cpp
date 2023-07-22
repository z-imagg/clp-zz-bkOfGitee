
namespace myNs1{
namespace myNs2{

struct User{
int func1(){
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
	char ch;//命名空间内的 函数 funOutImpl: 在结构体定义外实现函数体
  ch++;
  return ch;
}


char topOutFunc(float f1, double d2){
  int k;
	char c=f1>0 && d2<0?'a':'b';//无命名空间 的 顶层函数实现.
  return c;
}
