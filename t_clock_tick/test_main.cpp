#include <string>
#include <vector>
#include <thread>
#include <stdio.h>
#include <sstream>

using namespace std;


void GetCurrentThreadIdAsString(std::string& curThreadIdStr){
  // 获取当前线程的ID
  std::thread::id curThreadId = std::this_thread::get_id();
  // 线程ID转换为整数类型, 貌似依赖平台, 因此不转整形
//  std::uintptr_t threadIdInt = reinterpret_cast<std::uintptr_t>(curThreadId);

//
  // 将线程的唯一标识转换为字符串类型.  不同平台, 线程id转成的字符串样式不同.
//  std::string curThreadIdStr = std::to_string(curThreadId); //只在c++11中有 std::to_string ?

  // 将线程的唯一标识转换为字符串类型.  不同平台, 线程id转成的字符串样式不同.
  std::ostringstream outStrStream;
  outStrStream << curThreadId;
//  std::string curThreadIdStr = outStrStream.str();
  curThreadIdStr = outStrStream.str();

}

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
  std::string mainThreadIdStr;
  GetCurrentThreadIdAsString(mainThreadIdStr);
  const char *mainThreadIdCStr = mainThreadIdStr.c_str();
  printf("主线程的线程id:%s\n", mainThreadIdCStr);

  int arg1=9;
  char arg2='A';
  thread thread1(thread1_func,arg1, ref(arg2));
  thread1.join();


  vector<int> intS={5,-1,9,88,354,0,22};
  thread thread2(thread2_func,4, intS);
  thread2.join();

  return 0;
}