

#include <stdio.h>
#include <thread>
#include <sstream>
#include <atomic>
#include <fstream>

//////自定义线程id实现
// static std::atomic<int> 用作全局线程id计数器、  thread_local 线程id：  实现自定义进程内全时间唯一线程id
#define FirstThreadId 0
static std::atomic<int> globalThreadIdCounter(FirstThreadId);
int X__nextThreadId(){
  globalThreadIdCounter++;
  return globalThreadIdCounter;
}
#define ThreadIdInitVal -1
thread_local int currentThreadId=ThreadIdInitVal;//当前线程id
int X__curThreadId(){
  if(currentThreadId==ThreadIdInitVal){
    currentThreadId=X__nextThreadId();
  }else{
    return currentThreadId;
  }
}


///////本线程当前变量数目累积值

thread_local int t;//时钟
thread_local int sVarAllocCnt=0;//当前栈变量分配数目 sVarAllocCnt: currentStackVarAllocCnt
thread_local int sVarFreeCnt=0;//当前栈变量释放数目 sVarFreeCnt: currentStackVarFreeCnt
thread_local int sVarCnt=0;//当前栈变量数目（冗余） sVarCnt: currentStackVarCnt
thread_local int hVarAllocCnt=0;//当前堆对象分配数目 hVarAllocCnt: currentHeapObjAllocCnt, var即obj
thread_local int hVarFreeCnt=0;//当前堆对象释放数目 hVarFreeCnt: currentHeapObjcFreeCnt, var即obj
thread_local int hVarCnt=0;//当前堆对象数目（冗余）hVarCnt: currentHeapObjCnt, var即obj

///////当前滴答
class Tick{
public:
    int t;//时钟
    int sVarAllocCnt=0;//当前栈变量分配数目 sVarAllocCnt: currentStackVarAllocCnt
    int sVarFreeCnt=0;//当前栈变量释放数目 sVarFreeCnt: currentStackVarFreeCnt
    int sVarCnt=0;//当前栈变量数目（冗余） sVarCnt: currentStackVarCnt
    int hVarAllocCnt=0;//当前堆对象分配数目 hVarAllocCnt: currentHeapObjAllocCnt, var即obj
    int hVarFreeCnt=0;//当前堆对象释放数目 hVarFreeCnt: currentHeapObjcFreeCnt, var即obj
    int hVarCnt=0;//当前堆对象数目（冗余）hVarCnt: currentHeapObjCnt, var即obj
public:
    Tick(int _t,int _sVarAllocCnt, int _sVarFreeCnt, int _sVarCnt, int _hVarAllocCnt, int _hVarFreeCnt, int _hVarCnt)
    :
    t(_t),
    sVarAllocCnt(_sVarAllocCnt),
    sVarFreeCnt(_sVarFreeCnt),
    sVarCnt(_sVarCnt),
    hVarAllocCnt(_hVarAllocCnt),
    hVarFreeCnt(_hVarFreeCnt),
    hVarCnt(_hVarCnt)
    {
//      sVarCnt=sVarAllocCnt-sVarFreeCnt;
//      hVarCnt=hVarAllocCnt-hVarFreeCnt;
    }

    Tick( ){

    }

    void toString(std::string & line){
      char buf[128];
      sprintf(buf,"%d,%d,%d,%d,%d,%d,%d\n",t,sVarAllocCnt,sVarFreeCnt,sVarCnt,hVarAllocCnt,hVarFreeCnt,hVarCnt);
      line.append(buf);
    }
};

///////线程级滴答缓存
#define TickCacheSize 500
#define CacheIdxStart 0
class TickCache {
public:
    bool inited;
    Tick cache[TickCacheSize];
    int curEndIdx;
    std::ofstream fWriter;
    TickCache(){
      //构造函数被 "TLS init function for tickCache" 调用，发生在线程创建初始阶段，所以本函数最好少干事。
      inited=false;
    }

    void my_init(){
      if(inited){
        return;
      }

      inited=true;
      curEndIdx=CacheIdxStart;

      int curThreadId=X__curThreadId();
      std::string filePath=std::to_string(curThreadId);
      if(!fWriter.is_open()){
        fWriter.open(filePath);

        //刚打开文件时，写入标题行
        std::string title("滴答,栈生,栈死,栈净,堆生,堆死,堆净\n");
        fWriter << title ;
      }
    }
    ~TickCache(){
      if(!inited){
        return;
      }
      //此时估计是进程退出阶段，缓存无论是否满都要写盘，否则缓存中的数据就丢失了
      _flushIf(true);
      if(fWriter.is_open()){
        fWriter.close();
      }
    }
private:
    void _flushIf(bool condition){//由于本函数写了返回bool，但少了return，再次导致执行流乱跳。
      /////若条件满足, 则写盘 并 清空缓存.
      //若缓存满了
      if(condition){
        //写盘
        for(int i=0; i <=curEndIdx; i++){
          std::string line;
          cache[i].toString(line);
          fWriter << line ;
        }
        fWriter.flush();

        //清空缓存
        curEndIdx=CacheIdxStart;
      }
    }
public:
    void save(Tick & tick){
      if(!inited){
        my_init();
      }

      /////若缓存满了, 则写盘 并 清空缓存.
      bool full=curEndIdx==TickCacheSize-1;
      _flushIf(full);

      /////当前请求进缓存
      cache[curEndIdx]=tick;
      ++curEndIdx;

    }

};
thread_local TickCache tickCache;

/**
 *
 * @param _sVarAllocCnt  此次滴答期间， 栈变量分配数目
 * @param _sVarFreeCnt   此次滴答期间， 栈变量释放数目
 * @param _hVarAllocCnt   此次滴答期间， 堆对象分配数目
 * @param _hVarFreeCnt   此次滴答期间， 堆对象释放数目
 */
void X__t_clock_tick(int _sVarAllocCnt, int _sVarFreeCnt, int _hVarAllocCnt, int _hVarFreeCnt){

  //时钟滴答一下
  t++;

  //更新 当前栈变量分配数目
  sVarAllocCnt+=_sVarAllocCnt;

  //更新 当前栈变量释放数目
  sVarFreeCnt+=_sVarFreeCnt;

  //更新 当前栈变量数目 == 当前栈变量分配数目 - 当前栈变量释放数目
  sVarCnt= sVarAllocCnt - sVarFreeCnt;

  //更新 当前堆对象分配数目
  hVarAllocCnt+=_hVarAllocCnt;

  //更新 当前堆对象释放数目
  hVarFreeCnt+=_hVarFreeCnt;

  //更新 当前堆对象数目 == 当前堆对象分配数目 - 当前堆对象释放数目
  hVarCnt= hVarAllocCnt - hVarFreeCnt;

  //保存当前滴答
  Tick tick(t,sVarAllocCnt, sVarFreeCnt, sVarCnt, hVarAllocCnt,hVarFreeCnt,hVarCnt);
  tickCache.save(tick);


  return;
}

