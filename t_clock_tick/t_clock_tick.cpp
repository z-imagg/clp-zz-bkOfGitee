

#include <stdio.h>

thread_local int t;//时钟
thread_local int sVarAllocCnt=0;//当前栈变量分配数目 sVarAllocCnt: currentStackVarAllocCnt
thread_local int sVarFreeCnt=0;//当前栈变量释放数目 sVarFreeCnt: currentStackVarFreeCnt
thread_local int sVarCnt=0;//当前栈变量数目（冗余） sVarCnt: currentStackVarCnt
thread_local int hVarAllocCnt=0;//当前堆对象分配数目 hVarAllocCnt: currentHeapObjAllocCnt, var即obj
thread_local int hVarFreeCnt=0;//当前堆对象释放数目 hVarFreeCnt: currentHeapObjcFreeCnt, var即obj
thread_local int hVarCnt=0;//当前堆对象数目（冗余）hVarCnt: currentHeapObjCnt, var即obj
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

  printf("&滴答:%p,&栈当:%p,&堆当:%p,&栈生:%p,&栈死:%p,&堆生:%p,&堆死:%p\n", &t, &sVarCnt, &hVarCnt, &sVarAllocCnt, &sVarFreeCnt, &hVarAllocCnt, &hVarFreeCnt);
  //实测结果: 不同线程, 同一个全局变量 比如 t 的 地址 是 不同的，
  //    实测结论: thread_local修饰的全局变量 是 每线程一份独立变量， 即 不同线程的thread_local同一个全局变量地址不同。

  printf("滴答:%d,栈当:%d,堆当:%d,栈生:%d,栈死:%d,堆生:%d,堆死:%d\n", t, sVarCnt, hVarCnt, sVarAllocCnt, sVarFreeCnt, hVarAllocCnt, hVarFreeCnt);

  return;
}

