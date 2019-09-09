int max(int a,int b){
  return (a>b)?a:b;
}
int min(int a,int b){
  return (a<b)?a:b;
}
int abs(int a){
  return(a<0)?-a:a;
}
int strcmp(const char* s,const char *t){
  int i=0,j=0;
  while(s[i]!=0&&t[j]!=0){
    if(s[i]!=t[j]){
      return 1;
    }
    i++;
    j++;
  }
  if(s[i]!=t[j]) return 1;
  else return 0;
}
int strn_cmp(const char *s,const char *t,int size){
  for(int i=0;i<size;i++){
    if(s[i]!=t[i]){
      return 1;
    }if(t[i]==0){
      return 0;
    }
  }
  return 0;
}
int str_len(const char *s){
  int i=0;
  while(s[i]!=0)i++;
  return i;
}
