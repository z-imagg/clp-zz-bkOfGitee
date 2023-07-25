void f1(){
  int s;
  switch (s) {
    case 1:
    {
      [[gnu::fallthrough]];
    }
    case 2:
      break;
  }
}