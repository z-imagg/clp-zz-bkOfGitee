void func(){
  int chr;
  double sum;
  switch (chr) {
    case 1:sum+=chr;
    case 2:break;
    case 3:
      /*xxx*/ sum+=chr*6-chr%5 ;
      chr++  ;
  }
}