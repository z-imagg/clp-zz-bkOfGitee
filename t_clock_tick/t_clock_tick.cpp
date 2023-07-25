


int t;//时钟
int currentStackVarAllocCnt=0;//当前栈变量分配数目
int currentStackVarFreeCnt=0;//当前栈变量释放数目
int currentStackVarCnt=0;//当前栈变量数目（冗余）
int currentHeapObjAllocCnt=0;//当前堆对象分配数目
int currentHeapObjcFreeCnt=0;//当前堆对象释放数目
int currentHeapObjCnt=0;//当前堆对象数目（冗余）

/**
 *
 * @param stackVarAllocCnt  此次滴答期间， 栈变量分配数目
 * @param stackVarFreeCnt   此次滴答期间， 栈变量释放数目
 * @param heapObjAllocCnt   此次滴答期间， 堆对象分配数目
 * @param heapObjcFreeCnt   此次滴答期间， 堆对象释放数目
 */
void X__t_clock_tick(int stackVarAllocCnt, int stackVarFreeCnt, int heapObjAllocCnt, int heapObjcFreeCnt){

  //时钟滴答一下
  t++;

  //更新 当前栈变量分配数目
  currentStackVarAllocCnt+=stackVarAllocCnt;

  //更新 当前栈变量释放数目
  currentStackVarFreeCnt+=stackVarFreeCnt;

  //更新 当前栈变量数目 == 当前栈变量分配数目 - 当前栈变量释放数目
  currentStackVarCnt=currentStackVarAllocCnt-currentStackVarFreeCnt;

  //更新 当前堆对象分配数目
  currentHeapObjAllocCnt+=heapObjAllocCnt;

  //更新 当前堆对象释放数目
  currentHeapObjcFreeCnt+=heapObjcFreeCnt;

  //更新 当前堆对象数目 == 当前堆对象分配数目 - 当前堆对象释放数目
  currentHeapObjCnt=currentHeapObjAllocCnt-currentHeapObjcFreeCnt;

  return;
}

