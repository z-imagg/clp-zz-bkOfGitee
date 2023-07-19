// RUN: clang -cc1 -verify -load %shlibdir/libCodeStyleChecker%shlibext -plugin CSC %s 2>&1

// Verify that function names starting with upper case are reported as invalid

// expected-warning@+1 {{Function names should start with lower-case letter}}
void ClangTutorFuncBad();

void clangTutorFuncOK();

struct ClangTutorStruct {
  // expected-warning@+1 {{Function names should start with lower-case letter}}
  void ClangTutorMemberMethodBad();
  void clangTutorMemberMethodOK();
};

#include <stdio.h>
int main(int argc, char** argv){
  int age;
  printf("input age:");
  scanf("%d",&age);
  printf("your age:%d\n",age);

  int alive=false;
  int secret[100];
  for(int k =0; k <100; k++){
    if (secret[k] < age){
      alive=true;
      break;
    }
  }

  printf("are you still be alive?%d\n",alive);

  return 0;

}