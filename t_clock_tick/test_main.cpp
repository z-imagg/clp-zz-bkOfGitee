#include <stdio.h>

#include "zzz.h"
#include "yyy.h"

#define MAX_INT 9999

#define DECLARE__K int _k=0;


#include <cstdlib>

int f111(){
  static constexpr size_t AllocSize = 4096;
  char* NewMeta = static_cast<char *>(std::/*BUG04*/malloc(AllocSize));
  return 11;
}