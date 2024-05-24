#include "rntm_c__TmPnt_ThrLcl.h"

#include <iostream>
#include <pthread.h>
#include <unistd.h>

class TestClass{
public:
    static void* thread_function(void* _tmPnt_init_ptr);
};

void* TestClass::thread_function(void* _tmPnt_init_ptr){
  int* tmPnt_init_ptr = (int*) _tmPnt_init_ptr;
  int tmPnt_init = *tmPnt_init_ptr;

  int tmPnt_ThreadLocal_A=TL_TmPnt__get();
  TL_TmPnt__update(tmPnt_init);

  int tmPnt_ThreadLocal_B=TL_TmPnt__get();

  sleep(1);
  TL_TmPnt__update(tmPnt_init*2);
  int tmPnt_ThreadLocal_C=TL_TmPnt__get();
  int tmPnt_ThreadLocal_D=TL_TmPnt__get();

  TL_TmPnt__printPtr();
  printf(" tmPnt_ThreadLocal[A=%d, B=%d, C=%d, D=%d]; \n", tmPnt_ThreadLocal_A, tmPnt_ThreadLocal_B, tmPnt_ThreadLocal_C, tmPnt_ThreadLocal_D);

  pthread_exit(NULL);
}

int main(int argc, char** argv){


  pthread_t thread1, thread2, thread3;

  int _tmPnt_init_1 = 4, _tmPnt_init_2 = 7, _tmPnt_init_3 = 23;
  // 创建新线程
  if (pthread_create(&thread1, NULL, TestClass::thread_function, &_tmPnt_init_1 )) {
    perror("Error_creating_thread_1\n");
    return 1;
  }

  // 创建新线程
  if (pthread_create(&thread2, NULL, TestClass::thread_function,  &_tmPnt_init_2 )) {
    perror("Error_creating_thread_2\n");
    return 1;
  }

  // 创建新线程
  if (pthread_create(&thread3, NULL, TestClass::thread_function, &_tmPnt_init_3 )) {
    perror("Error_creating_thread_3\n");
    return 1;
  }


  sleep(7);
  return 0;
}

/**输出举例
TL_TmPnt@0x6d3ff63c
 tmPnt_ThreadLocal[A=-1, B=4, C=8, D=8];
TL_TmPnt@0x6c3fd63c
 tmPnt_ThreadLocal[A=-1, B=23, C=46, D=46];
TL_TmPnt@0x6cbfe63c
 tmPnt_ThreadLocal[A=-1, B=7, C=14, D=14];
*/