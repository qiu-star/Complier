#include "symbol.h"

/**
 * 如果符号类型是function
 */ 
void insertSymTabFun(string name, int type, int value, int para, int address)
{
    //查找分程序中是否有与它名字相同的
    int i;
    for(i = 0;i < symTable.totalPro; i++)
    {
        if(strcmp(symTable.element[symTable.proIndex[i]].name, name) == 0)
        {
            fprintf(stderr,"function name complict!\n"); 
            exit(1);
        }
    }

    symTable.proIndex[symTable.totalPro] = symTable.length;
    symTable.totalPro += 1; 
    symTable.element[symTable.length].name = String(name);
    symTable.element[symTable.length].type = type;
    symTable.element[symTable.length].value = value;//函数的value指的是返回值为char or int
    symTable.element[symTable.length].para = para;//参数个数
    symTable.element[symTable.length].address = address;
    symTable.length += 1;
}

/**
 * 如果符号类型是const var para
 */ 
void insertSymTabElse(string name, int type, int value, int para, int address)
{
    //从当前分程序中查找是否有与它名字相同的
    int index = (symTable.totalPro-1)>0? (symTable.totalPro-1):0;
    for(int i = symTable.proIndex[index]; i < symTable.length; i++)
    {
        if(strcmp(symTable.element[i].name, name) == 0)
        {
            fprintf(stderr,"const/var/para name complict!\n"); 
            exit(1);
        }
    }

    symTable.element[symTable.length].name = String(name);
    symTable.element[symTable.length].type = type;
    symTable.element[symTable.length].value = value;//函数的value指的是返回值为char or int
    symTable.element[symTable.length].para = para;//参数个数
    symTable.element[symTable.length].address = address;
    symTable.length += 1;
}

/**
 * 暂时不支持全局变量；解决方法：可以定优先级，首先定义全局，再局部
 */ 
void insertSymTab(string name, int type, int value, int para, int address)
{
    if(symTable.length >= MAX)
    {
        fprintf(stderr,"out of symbol table index!\n"); 
        exit(1);
    }

    if(type == 2)//如果是function
    {
        insertSymTabFun(name, type, value, para, address);
    }
    else//如果是const var para
    {
        insertSymTabElse(name, type, value, para, address);
    }
    
}

void printSymTab()
{
    FILE *f = fopen("result/symbolList.txt","w");
    printf("----------------------符号表----------------------\n");
    for(int i = 0; i < symTable.length; i++)
    {
        char* tmp = (char*)malloc(sizeof(char)*128);
        sprintf(tmp,"name: %10s type: %2d value: %2d param: %2d address: %2d\n",symTable.element[i].name,symTable.element[i].type,symTable.element[i].value,symTable.element[i].para,symTable.element[i].address);
        fwrite(tmp,sizeof(char),strlen(tmp),f);
    }
}