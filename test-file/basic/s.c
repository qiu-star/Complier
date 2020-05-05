int change[10];
int cmp(int a, int b)
{
    return (a+b);
}
void main(){
   cmp(1,2);
   change[0+1]=1;
   change[1+2]=2;
   change[2+3]=change[1+0]+change[1+1+1];
   if(change[1]<change[1+2])
   printf(change[5]);
   else 
   printf("false",change[5]);
}
