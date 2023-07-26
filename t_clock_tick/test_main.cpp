#include <string>
#include <vector>
#include <thread>
#include <stdio.h>

using namespace std;

void thread1_func(int arg1, char & arg2){
  float sum,max=0;
  sum=(float)(arg1+arg2);
  max=max>sum?max:sum;

  bool end;
  end=sum<max;

  printf("threa1_func:arg1:%d,arg2:%d,sum:%f,max:%f,end:%d\n",arg1,arg2,sum,max,end);
  return;
}

void thread2_func(int k, vector<int> arg2){
  int size=arg2.size();
  arg2[k]=size;
  int sum=0;
  for(int z=0; z < size; z++){
    sum+=arg2[z];
  }
  printf("threa2_func:sum:%d\n",sum);

  return;
}
int main(int argc, char** argv){
  int arg1=9;
  char arg2='A';
  thread thread1(thread1_func,arg1, ref(arg2));
  thread1.join();


  vector<int> intS={5,-1,9,88,354,0,22};
  thread thread2(thread2_func,4, intS);
  thread2.join();

  return 0;
}