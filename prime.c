#include"apifunc.h"
#define MAX 50000
void HariMain(){
  char is_prime[MAX],s[8];
  for(int i=0;i<MAX;i++){
    is_prime[i]=0;
  }
  is_prime[0]=is_prime[1]=1;
  for(int i=2;i<MAX;i++){
    if(is_prime[i]==0){
      my_sprintf(s,"%d ",i);
      api_putstr0(s);
      for(int j=2;i*j<MAX;j++){
        is_prime[i*j]=1;
      }
    }
  }
  api_end();
}
