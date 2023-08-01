// cp /pubx/clang-ctk/t_clock_tick/t_clock_tick.h /pubx/llvm-project/llvm/include/t_clock_tick.h
#ifndef _T_CLOCK_TICK_H
#define _T_CLOCK_TICK_H




// extern int t;//时钟
// extern int sVarAllocCnt;//当前栈变量分配数目
// extern int sVarFreeCnt;//当前栈变量释放数目
// extern int sVarCnt;//当前栈变量数目（冗余）
// extern int hVarAllocCnt;//当前堆对象分配数目
// extern int hVarFreeCnt;//当前堆对象释放数目
// extern int hVarCnt;//当前堆对象数目（冗余）

/**
 *
 * @param stackVarAllocCnt  此次滴答期间， 栈变量分配数目
 * @param stackVarFreeCnt   此次滴答期间， 栈变量释放数目
 * @param heapObjAllocCnt   此次滴答期间， 堆对象分配数目
 * @param heapObjcFreeCnt   此次滴答期间， 堆对象释放数目
 */
#ifndef __cplusplus
//如果当前是.c文件包含了此.h,  则强制使用#函数名到c++名字
#define X__t_clock_tick  _Z15X__t_clock_tickiiiiPi
#define X__funcEnter  _Z12X__funcEnterv
#define X__funcReturn _Z13X__funcReturnPi
#endif

void X__t_clock_tick(int dSVarAC, int dSVarFC, int dHVarAC, int dHVarFC, int* topFuncSVarCnt_ptr);

/////函数X__funcEnter、X__funcReturn用于:  return语句 应该释放 本函数当前已经申请的所有栈变量。
/**
 * 函数X__funcEnter插入在函数第一句话之前
 */
void X__funcEnter( );
/**
 * 函数X__funcReturn插入在函数的每条return语句之前，以及void函数的末尾之前
 */
void X__funcReturn(int* topFuncSVarCnt_ptr );
#endif //_T_CLOCK_TICK_H
