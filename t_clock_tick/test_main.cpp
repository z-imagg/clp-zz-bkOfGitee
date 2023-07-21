#include <stdio.h>

#include "zzz.h"
#include "yyy.h"

#define MAX_INT 9999

#define DECLARE__K int _k=0;


int f111(){
  return 11;
}
int f222(){
  return /*BUG1*/ f111();//BUG1, 待修复
}

bool maxFunc(int* pA, int* pB, int* pMax){
  if (pA == nullptr || /*BUG2*/ !f111()){//BUG1, 待修复

  }

  int x=0;
  for(int zk=0; zk<10; zk++)
    if (zk%2==0) x+=zk;

  while(false)
    x--;

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


  return 0;

}