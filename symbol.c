#include "symbol.h"

/**
 * 如果符号类型是function
 */ 
void insertSymTabFun(string name, int type, int value, int para)
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
    symTable.length += 1;
}

/**
 * 如果符号类型是const var para
 */ 
void insertSymTabElse(string name, int type, int value, int para)
{
    //从当前分程序中查找是否有与它名字相同的
    for(int i = symTable.proIndex[symTable.totalPro-1]; i < symTable.length; i++)
    {
        if(strcmp(symTable.element[i].name, name) == 0)
        {
            fprintf(stderr,"const/var/para name complict!\n"); 
            exit(1);
        }
    }

    symTable.proIndex[symTable.totalPro] = symTable.length;
    symTable.totalPro += 1; 
    symTable.element[symTable.length].name = String(name);
    symTable.element[symTable.length].type = type;
    symTable.element[symTable.length].value = value;//函数的value指的是返回值为char or int
    symTable.element[symTable.length].para = para;//参数个数
    symTable.length += 1;
}

void insertSymTab(string name, int type, int value, int para)
{
    if(symTable.length >= MAX)
    {
        fprintf(stderr,"out of symbol table index!\n"); 
        exit(1);
    }

    if(type == 2)//如果是function
    {
        insertSymTabFun(name, type, value, para);
    }
    else//如果是const var para
    {
        insertSymTabElse(name, type, value, para);
    }
    
}

void printSymTab()
{
    for(int i = 0; i < symTable.length; i++)
    {
        printf("name: %s\ttype: %d\tvalue: %d\tparam: %d\n",symTable.element[i].name,symTable.element[i].type,symTable.element[i].value,symTable.element[i].para);
    }
}