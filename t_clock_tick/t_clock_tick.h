#ifndef _T_CLOCK_TICK_H
#define _T_CLOCK_TICK_H




extern int t;//时钟
extern int currentStackVarAllocCnt;//当前栈变量分配数目
extern int currentStackVarFreeCnt;//当前栈变量释放数目
extern int currentStackVarCnt;//当前栈变量数目（冗余）
extern int currentHeapObjAllocCnt;//当前堆对象分配数目
extern int currentHeapObjcFreeCnt;//当前堆对象释放数目
extern int currentHeapObjCnt;//当前堆对象数目（冗余）

/**
 *
 * @param stackVarAllocCnt  此次滴答期间， 栈变量分配数目
 * @param stackVarFreeCnt   此次滴答期间， 栈变量释放数目
 * @param heapObjAllocCnt   此次滴答期间， 堆对象分配数目
 * @param heapObjcFreeCnt   此次滴答期间， 堆对象释放数目
 */
extern  void X__t_clock_tick(int stackVarAllocCnt, int stackVarFreeCnt, int heapObjAllocCnt, int heapObjcFreeCnt);

#endif //_T_CLOCK_TICK_H
