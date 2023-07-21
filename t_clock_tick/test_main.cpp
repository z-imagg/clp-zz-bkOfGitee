#include <stdio.h>

#include "zzz.h"
#include "yyy.h"

int f111(){
  return 11;
}
int f222(){
  return /*BUG1*/ f111();//BUG1, 待修复
}

bool maxFunc(int* pA, int* pB, int* pMax){
  if (pA == nullptr || /*BUG2*/ !f111()){//BUG1, 待修复

  }

  return true;
}
