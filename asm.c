#include "asm.h"

void initLex(char *fname);
void getSym();

FILE *mipsf;

//全局变量/常量表
GlobalTab globalTab;
//局部变量/常量表
LocalTab localTab;

//四元式表
FourVarCodeTab midcodeTab;
//目前扫描到的位置（四元式表）
int pos = 0;

/**
 * 定义全局变量
 */ 
void asmGlobalData()
{
    char tmp[] = ".data\n";
    fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
    //常量定义
    asmGlobalConstDefine();
    //变量定义
    asmGlobalVarDefine();
    //对print函数里的字符串进行处理
    asmPrintStringDefine();

    char t[512];
    sprintf(t, ".text\n");
    fwrite(t,sizeof(char),strlen(t),mipsf);
    sprintf(t, ".globl main\n");
    fwrite(t,sizeof(char),strlen(t),mipsf);
    sprintf(t, "\t\tj\tmain\n");
    fwrite(t,sizeof(char),strlen(t),mipsf);
}

void asmGlobalConstDefine()
{
    int i,len;
    char tmp[512];
    for(i =0; strcmp("const",midcodeTab.element[i].op) == 0 && i < midcodeTab.length; i++)
    {
        //const四元式const, int, 12, a
        len = globalTab.len;
        globalTab.g[len].name = midcodeTab.element[i].rst;
        globalTab.g[len].value = midcodeTab.element[i].num_b;
        //g:	.word	5
        sprintf(tmp,"%s:\t.word\t%s\n",globalTab.g[len].name,globalTab.g[len].value);
        fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
        globalTab.len = ++len;
    }
    pos = i;
}

void asmGlobalVarDefine()
{
    int i = pos,len;
    char tmp[512];
    while( (strcmp(midcodeTab.element[i].op, "int") == 0 || 
        strcmp(midcodeTab.element[i].op, "char") == 0 ||
        strcmp(midcodeTab.element[i].op, "inta") == 0 ||
        strcmp(midcodeTab.element[i].op, "chara") == 0) && i < midcodeTab.length)
    {
        len = globalTab.len;
        globalTab.g[len].name = midcodeTab.element[i].rst;
        globalTab.g[len].value = "";
        if(strcmp(midcodeTab.element[i].op, "inta") == 0 ||
          strcmp(midcodeTab.element[i].op, "chara") == 0)
        {
            //a:	.space	20
            sprintf(tmp,"%s:\t.space\t%d\n",globalTab.g[len].name,atoi(midcodeTab.element[i].num_b)*4);
        }
        else
        {
            sprintf(tmp,"%s:\t.space\t4\n",globalTab.g[len].name);
        }
        fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
        i++;
        globalTab.len = ++len;
    }
    pos = i;
}

void asmPrintStringDefine()
{
    int i, cnt = 0;
    char tmp[512];
    for(i = pos; i < midcodeTab.length; i++)
    {
        if(strcmp(midcodeTab.element[i].op, "prtf") == 0)
        {
            //printf("11111111111");
            if(strcmp(midcodeTab.element[i].num_a, "") != 0)
            {
                int len = globalTab.len;
                char name[20];
                sprintf(name,"$string%d",cnt);
                globalTab.g[len].name = String(name);
                globalTab.g[len].value = midcodeTab.element[i].num_a;
                sprintf(tmp, "%s:\t.asciiz\t\"%s\"\n", name, midcodeTab.element[i].num_a);
                fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
                //定义好常量后，把四元式从prtf, false, $_13, int换成prtf,$string1,$_13,int
                midcodeTab.element[i].num_a = globalTab.g[len].name;
                globalTab.len = ++len;
                cnt++;
            }
        }
    }
}

void printGlobalTable()
{
    for(int i = 0; i < globalTab.len; i++)
    {
        printf("name: %s\t\tvalue:%s\n",globalTab.g[i].name,globalTab.g[i].value);
    }
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr,"usage: a.out filename\n"); 
        exit(1);
    }

    //初始化
    initFourVarCodeTab();
    initSymTab();
    initLex(argv[1]);

    //语法分析
    getSym();
    programme();

    //语法分析结果输出
    printSymTab();
    printFourVarCodeTab();

    midcodeTab = getMidCode();
    //程序设定：先定义全局变量、然后是程序
    mipsf = fopen("result/mips.txt","w");

    asmGlobalData();
    
    printGlobalTable();
    return 0;
}