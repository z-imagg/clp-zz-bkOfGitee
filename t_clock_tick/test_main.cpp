#include "t_clock_tick.h"
#include <string>
#include <vector>
#include <thread>
#include <stdio.h>
#include <sstream>

using namespace std;


void GetCurrentThreadIdAsString(std::string& curThreadIdStr){
  // 获取当前线程的ID
  X__t_clock_tick(/*栈生*/1, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
std::thread::id curThreadId = std::this_thread::get_id();
  // 线程ID转换为整数类型, 貌似依赖平台, 因此不转整形
//  std::uintptr_t threadIdInt = reinterpret_cast<std::uintptr_t>(curThreadId);

//
  // 将线程的唯一标识转换为字符串类型.  不同平台, 线程id转成的字符串样式不同.
//  std::string curThreadIdStr = std::to_string(curThreadId); //只在c++11中有 std::to_string ?

  // 将线程的唯一标识转换为字符串类型.  不同平台, 线程id转成的字符串样式不同.
  X__t_clock_tick(/*栈生*/1, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
std::ostringstream outStrStream;
  X__t_clock_tick(/*栈生*/0, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
outStrStream << curThreadId;
//  std::string curThreadIdStr = outStrStream.str();
  X__t_clock_tick(/*栈生*/0, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
curThreadIdStr = outStrStream.str();

X__t_clock_tick(/*栈生*/0, /*栈死*/2, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
}

void thread1_func(int arg1, char & arg2){
  X__t_clock_tick(/*栈生*/1, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
std::string thread1IdStr;
  X__t_clock_tick(/*栈生*/0, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
GetCurrentThreadIdAsString(thread1IdStr);
  X__t_clock_tick(/*栈生*/0, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
printf("thread1 线程id:%s\n", thread1IdStr.c_str());

  X__t_clock_tick(/*栈生*/2, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
float sum,max=0;
  X__t_clock_tick(/*栈生*/0, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
sum=(float)(arg1+arg2);
  X__t_clock_tick(/*栈生*/0, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
max=max>sum?max:sum;

  X__t_clock_tick(/*栈生*/1, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
bool end;
  X__t_clock_tick(/*栈生*/0, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
end=sum<max;

  X__t_clock_tick(/*栈生*/0, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
printf("threa1_func:arg1:%d,arg2:%d,sum:%f,max:%f,end:%d\n",arg1,arg2,sum,max,end);
  X__t_clock_tick(/*栈生*/0, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
X__t_clock_tick(/*栈生*/0, /*栈死*/4, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
return;
}

void thread2_func(int k, vector<int> arg2){
  X__t_clock_tick(/*栈生*/1, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
std::string thread2IdStr;
  X__t_clock_tick(/*栈生*/0, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
GetCurrentThreadIdAsString(thread2IdStr);
  X__t_clock_tick(/*栈生*/0, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
printf("thread2 线程id:%s\n",  thread2IdStr.c_str());

  X__t_clock_tick(/*栈生*/1, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
int size=arg2.size();
  X__t_clock_tick(/*栈生*/0, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
arg2[k]=size;
  X__t_clock_tick(/*栈生*/1, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
int sum=0;
  X__t_clock_tick(/*栈生*/0, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
for(int z=0; z < size; z++){
    X__t_clock_tick(/*栈生*/0, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
sum+=arg2[z];
  }
  X__t_clock_tick(/*栈生*/0, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
printf("threa2_func:sum:%d\n",sum);

  X__t_clock_tick(/*栈生*/0, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
X__t_clock_tick(/*栈生*/0, /*栈死*/3, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
return;
}
int main(int argc, char** argv){
  X__t_clock_tick(/*栈生*/1, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
std::string mainThreadIdStr;
  X__t_clock_tick(/*栈生*/0, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
GetCurrentThreadIdAsString(mainThreadIdStr);
  X__t_clock_tick(/*栈生*/0, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
printf("主线程 线程id:%s\n", mainThreadIdStr.c_str());

  X__t_clock_tick(/*栈生*/1, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
int arg1=9;
  X__t_clock_tick(/*栈生*/1, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
char arg2='A';
  X__t_clock_tick(/*栈生*/1, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
thread thread1(thread1_func,arg1, ref(arg2));
  X__t_clock_tick(/*栈生*/0, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
thread1.detach();


  X__t_clock_tick(/*栈生*/1, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
vector<int> intS={5,-1,9,88,354,0,22};
  X__t_clock_tick(/*栈生*/1, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
thread thread2(thread2_func,4, intS);
  X__t_clock_tick(/*栈生*/0, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
thread2.detach();

  X__t_clock_tick(/*栈生*/0, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
std::this_thread::sleep_for(2s);

  X__t_clock_tick(/*栈生*/0, /*栈死*/0, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
X__t_clock_tick(/*栈生*/0, /*栈死*/6, /*堆生*/0, /*堆死*/0);//TraverseCompoundStmt
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
