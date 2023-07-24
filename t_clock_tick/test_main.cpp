

char topOutFunc(float f1, double d2){
  int diff;
  int arr[]={8,0,-1,99};
//  for(int q: arr)
    for(int q=0; q< 8 ; q++)
    if (q>0)//if1
      if(diff>9)//if2
      diff+=q;
      else//else1
        if (q+diff>q*diff)//if3
        arr[q]++;
        else//else2
        if(false){//if4
          arr[0]-=diff;
        }else//else3
          arr[diff]*=7;

  
}
