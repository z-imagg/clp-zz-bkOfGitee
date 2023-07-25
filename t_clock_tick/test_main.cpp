#define LLVM_FALLTHROUGH [[gnu::fallthrough]]

void f1(){
  int s;
  switch (s) {
    case 1:
    {
      int age;
      LLVM_FALLTHROUGH;
    }
    case 2:
      break;
  }
}