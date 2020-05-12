int change[10];
int cmp(int a, int b)
{
    /* const int cnt = 0; */
    char tmp[12];
    int c,d;
    if(a>b)
    {
	a = b + 1;
        d = a - b;
        c = a*2-b;
    }
    else a = 0;
    c = a;
    return (c+b);
}
void main(){
   int f,a,b;
   a = 1;
   b = 2;
   f = cmp(a,b);
   change[0+1]=1;
   change[1+2]=2;
   change[2+3]=change[1+0]+change[1+1+1];
   if(change[1]<change[1+2])
   printf(change[5]);
   else 
   printf("false",change[5]);
}
