
#define LLVM_FALLTHROUGH [[gnu::fallthrough]]

char topOutFunc000(float f1, double d2){
  char ch;
  int val;
  switch(val){
    case 1:{
      break;
    }
    case2:
      val++;
    case 3: {
      ch+=val;
    --ch;  
    LLVM_FALLTHROUGH; //本fallthrough后面不能插入，注意本fallthrough此时并不在块尾
  case 4:
    

    if (val>9)
      break;

    int x,  y, z=1;
    y+=z;
    break;
    }
    
  }
}

 