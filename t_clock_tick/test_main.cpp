#include <string>
#include <vector>
#include <thread>
#include <stdio.h>
#include <sstream>

using namespace std;

void empty_func(int z, float y){

}
class Person{

};
class User{
public:
    ~User(){
      int zero=9;
      float x=3;
    }
    operator Person() const;
};

User::operator Person() const {
  return Person();
}

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
  std::string thread1IdStr;
  GetCurrentThreadIdAsString(thread1IdStr);
  printf("thread1 线程id:%s\n", thread1IdStr.c_str());

  float sum,max=0;
  sum=(float)(arg1+arg2);
  max=max>sum?max:sum;

  bool end;
  end=sum<max;

  printf("threa1_func:arg1:%d,arg2:%d,sum:%f,max:%f,end:%d\n",arg1,arg2,sum,max,end);
  return;
}

void thread2_func(int k, vector<int> arg2){
  auto sumFunc = [&arg2](const int k) -> bool {
    int age;
    float sum;
    for(int i=0; i <=k ; i++){
      sum+=age;
    }
    return sum;
  };
  std::string thread2IdStr;
  GetCurrentThreadIdAsString(thread2IdStr);
  printf("thread2 线程id:%s\n",  thread2IdStr.c_str());

  sumFunc(100);
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
  printf("主线程 线程id:%s\n", mainThreadIdStr.c_str());

  int arg1=9;
  char arg2='A';
  thread thread1(thread1_func,arg1, ref(arg2));
  thread1.detach();


  vector<int> intS={5,-1,9,88,354,0,22};
  thread thread2(thread2_func,4, intS);
  thread2.detach();

  std::this_thread::sleep_for(2s);

  return 0;
}
/**
主线程 线程id:140658717438976
thread2 线程id:140658702542400
thread1 线程id:140658710935104

c++线程id: 单进程内单时刻唯一、单进程内两异时间段可相同
 1. 在单一进程内任意单一时刻 全部活着的线程id 唯一。
 2. 在单一进程内,  单一线程id 可以存活在 两个不同的时间段内。

//主线程 将 线程1 脱离  后， 主线程 又将 线程2 脱离。
 即 主线程、线程1、线程2  在时间轴上 极大概率重合。  因此  三线程id 不能重复。


 */
