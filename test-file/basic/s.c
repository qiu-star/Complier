int change[10];
int cmp(int a, int b)
{
    char tmp[12];
    int c,d;
    if(a>b)
    {a = b;}
    else a = 0;
    c = a;
    return (c+b);
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
