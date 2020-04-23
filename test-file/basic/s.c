const int a=1 , b=2;
int c, d;
int f[10];
int i;
char m;
int g;
g=32;
if(a == b)
{
    c = 1;
    m = 'k';
    scanf(c,d,i);
    printf("abc");
    printf("hello %d!",a);
    printf("%d",a+b-c*d+f[0]);
    printf(c);
    printf(m);
    return (a+b+c);
}
else
{
    f[0] = 12;
    f[1] = 10;
    d = f[1+a*2];
    return;
    for(i=0;i<f[0]+f[1];i=i+3)
    {
	c = c + 1;
	d = c;
    }
}

int cmp(int a, int b)
{
    if(a > b) return (1);
    else return (0);
}
