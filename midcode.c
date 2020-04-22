#include "midcode.h"

FourVarCodeTab fourVarCodeTab;
int varNum = 0;//自动生成的变量个数（用于返回自动生成的变量名

void initFourVarCodeTab()
{
    fourVarCodeTab.length=0;
}

void insertIntIntoFourVarCodeTab(char* op, char* a, int b, char* result)
{
    int length = fourVarCodeTab.length;
    if (length >= MAX)
    {
        fprintf(stderr,"out of four var table index!\n"); 
        exit(1);
    }

    fourVarCodeTab.element[length].op = String(op); 
    fourVarCodeTab.element[length].num_a = String(a);
    fourVarCodeTab.element[length].num_b = itoa(b);
    fourVarCodeTab.element[length].rst = String(result);
    fourVarCodeTab.length += 1;
}

void insertStringIntoFourVarCodeTab(char* op, char* a, char* b, char* result)
{
    int length = fourVarCodeTab.length;
    if (length >= MAX)
    {
        fprintf(stderr,"out of four var table index!\n"); 
        exit(1);
    }

    fourVarCodeTab.element[length].op = String(op); 
    fourVarCodeTab.element[length].num_a = String(a);
    fourVarCodeTab.element[length].num_b = String(b);
    fourVarCodeTab.element[length].rst = String(result);
    fourVarCodeTab.length += 1;
}

/**
 * 生成一个变量名
 * @return 返回生成的变量名
 */ 
string generateVar()
{
    char* var = (char*)malloc(sizeof(char)*16);
    sprintf(var,"$_%d", varNum);
    varNum++;
    return var;
}

void printFourVarCodeTab()
{
    FILE *f = fopen("result/midCode.txt","w");
    printf("----------------------四元式----------------------\n");
    for(int i = 0; i < fourVarCodeTab.length; i++)
    {
        char* tmp = (char*)malloc(sizeof(char)*128);
        sprintf(tmp,"%s, %s, %s, %s\n",fourVarCodeTab.element[i].op,fourVarCodeTab.element[i].num_a,fourVarCodeTab.element[i].num_b,fourVarCodeTab.element[i].rst);
        fwrite(tmp,sizeof(char),strlen(tmp),f);
    }
}