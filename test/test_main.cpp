#include <stdio.h>

#define MAX_INT 9999

#define DECLARE__K int _k=0;


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

bool maxFunc(int* pA, int* pB, int* pMax){
  if (pA==NULL || pB==NULL){
    return false;
  }
  int A=*pA;
  int B=*pB;
  if(A>B){
    (*pMax)=A;
  }else{
    (*pMax)=B;
  }
  return true;
}

template<typename T>
void funcRange(T start, T end, T step, T& result,  T const zero, T (*binaryFunc)(T a, T b)){
  if(binaryFunc==NULL){
    return  ;
  }
  result=zero;
  for(T k=start; k<=end; k+=step){
    char zzz;
    result=binaryFunc(result,k);
  }
  return  ;
}

int intSum(int a, int b){
  return a+b;
}


int intPower(int a, int b){
  int power=1;
  for(int k=0; k < a; k++){
    power*=b;
  }
  return power;
}

float floatPower(float a, float b){
  printf("__%f,%f\n",a,b);
  float power=1.0;
  for(int k=0; k < a; k++){
    power*=b;
    power = ((int)power)%100;
  }
  printf("=%f\n",power);
  return power;
}
int main(int argc, char** argv){
  DECLARE__K
  
  int x, y, max=MAX_INT;
  printf("input x,y:");
  scanf("%d,%d",&x,&y);
  bool resultMax = maxFunc(&x,&y,&max);
  if(resultMax)
    printf("maxFunc(%d,%d)=%d\n",x,y,max);
  else
    printf("call maxFunc failed\n");


  int start=0,end=20,step=1,zero=0,resultF1;
  funcRange(start,end,step,resultF1,zero,intSum);
  printf("funcRange(%d,%d,%d,resultF1,%d,intSum)=%d\n",start,end,step,zero,resultF1);

  start=1,end=5,step=1,zero=1;
  int resultF2;
  funcRange(start,end,step,resultF2,zero,intPower);
  printf("funcRange(%d,%d,%d,resultF2,%d,intPower)=%d\n",start,end,step,zero,resultF2);

  float startF=1,endF=6,stepF=1,zeroF=1,resultF3;
  funcRange(startF,endF,stepF,resultF3,zeroF,floatPower);
  printf("funcRange(%f,%f,%f,resultF3,%f,floatPower)=%f\n",startF,endF,stepF,zeroF,resultF3);


  printf("main_end,t=%d\n",t);
  return 0;

}