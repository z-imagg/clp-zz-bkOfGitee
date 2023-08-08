#define CASE_SAME(x) case x:
class MyClass {
public:
    static int ZERO;
public:

  char* switchDemo(int cnt, short chr){
    double sum=1.*cnt*chr;
    float PI;
    start: char L;
    switch(chr){
      case -4:  case -3:
      case -2:case-1:
      sum++;
      break;
      case 0:
      {
        L=(cnt+chr)%50;
        return &L+1;//case0 结束位置左移1
      }


      case 1:
        int/*此非case1结束*/ unit;//case1共3行单语句, 这里是case1的第1行单语句
        unit=18;
        return 0;//case1 结束位置
        CASE_SAME(-100)
      case 2:
        cnt/*此非case2结束*/=L*L;
        cnt++;
        break;//case2 结束
      case 3:
        return/*此非case3结束*/ &L;//case3 结束
      case 4:
      {
        L++;
        sum=L+sum;
        if (L)  goto start;//这里并不是case4结束, 这里是case4的块1结束, case4 : 块1+散语句，
      }
      PI=3.14;
      return (char*)0;
      case 5:{
        int zB;
        break;//case5 结束位置左移1
      }//case5语句结束, 但后面还有一行注释
      ////////
      case 6:{
        int uuuuu;
        break;//case6 结束位置左移1
      }//case6 结束
      default:
        sum/*此非default结束*/+=chr*cnt;
        return &L;//default结束
    }
    return 0;
  }
};

int MyClass::ZERO=0;

