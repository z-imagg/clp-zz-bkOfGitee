
namespace myNs1{
namespace myNs2{

struct User{
int f111(){
return 11;
}
int f222(){
return  f111(); 
}

int funOutImpl();
};

}

}

int myNs1::myNs2::User::funOutImpl() {
  char ch;
  ch++;
  return ch;
}


char topOutFunc(float f1, double d2){
  char c=f1>0 && d2<0?'a':'b';
  return c;
}
