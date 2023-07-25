// cp /pubx/clang-ctk/t_clock_tick/t_clock_tick.h /pubx/llvm-project/llvm/include/t_clock_tick.h
#ifndef _T_CLOCK_TICK_H
#define _T_CLOCK_TICK_H




// extern int t;//时钟
// extern int currentStackVarAllocCnt;//当前栈变量分配数目
// extern int currentStackVarFreeCnt;//当前栈变量释放数目
// extern int currentStackVarCnt;//当前栈变量数目（冗余）
// extern int currentHeapObjAllocCnt;//当前堆对象分配数目
// extern int currentHeapObjcFreeCnt;//当前堆对象释放数目
// extern int currentHeapObjCnt;//当前堆对象数目（冗余）

/**
 *
 * @param stackVarAllocCnt  此次滴答期间， 栈变量分配数目
 * @param stackVarFreeCnt   此次滴答期间， 栈变量释放数目
 * @param heapObjAllocCnt   此次滴答期间， 堆对象分配数目
 * @param heapObjcFreeCnt   此次滴答期间， 堆对象释放数目
 */
#ifndef __cplusplus
//如果当前是.c文件包含了此.h,  则强制使用#函数名到c++名字
#define X__t_clock_tick  _Z15X__t_clock_tickiiii
#endif

void X__t_clock_tick(int stackVarAllocCnt, int stackVarFreeCnt, int heapObjAllocCnt, int heapObjcFreeCnt);

#endif //_T_CLOCK_TICK_H
