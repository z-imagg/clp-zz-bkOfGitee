#include "rntm_c__TmPnt_ThrLcl.h"

#include <stdio.h>

void func1(){
TL_TmPnt__update(4);
}

int main(int argc, char** argv){
func1();
int tmPnt=TL_TmPnt__get();
printf("tmPnt=%d\n",tmPnt);

}