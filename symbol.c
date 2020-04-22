#include "symbol.h"

SymbolTable symTable;

int isConst=0;
int isArr=0;

int getIsConst()
{
    return isConst;
}

void setIsConst(int f)
{
    isConst = f;
}

void initSymTab()
{
    symTable.length = 0;
    symTable.totalPro = 1;
}

/**
 * 如果符号类型是function
 */ 
void insertSymTabFun(string name, int type, int value, int para, int address)
{
    //查找分程序中是否有与它名字相同的
    int i;
    //i=0分配给全局变量
    for(i = 1;i < symTable.totalPro; i++)
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
    int index = (symTable.totalPro-1);
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
 * 首先定义全局，全局变量存在最前面，再局部
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

/**
 * @param name 符号名
 * @param ifFunction 该符号是否为函数
 * @param paraNum 如果是函数，那么调用该函数的参数个数是多少；如果不是函数，该参数为是否为数组，如果是数组为1,不是数组为0,不确定为-1
 */ 
int searchSymTab(string name, int ifFunction, int paraNum)
{
    if(ifFunction)
    {
        //在分程序名中查找
        int i;
        for(i = 0; i < symTable.totalPro; i++)
        {
            if(strcmp(name, symTable.element[symTable.proIndex[i]].name) == 0)
            {
                break;
            }
        }
        if(i == symTable.totalPro) return 0;
        //比对参数个数
        if(paraNum != symTable.element[symTable.proIndex[i]].para) return 0;
        //@TODO 比对参数类型
        return 1;
    }
    else
    {
        //查找变量
        int i;
        int index = (symTable.totalPro-1);
        for(i = symTable.proIndex[index]; i < symTable.length; i++)
        {
            if(strcmp(name, symTable.element[i].name) == 0) break;
        }
        if(i == symTable.length)//在全局变量中找，也就是在程序段数为0的区域找
        {
            int j;
            for(j = 0; i < symTable.proIndex[1]; i++)
            {
                if(strcmp(name, symTable.element[i].name)) break;
            }
            if(j == symTable.proIndex[1]) return 0;
            //是不是数组
            if(paraNum == 1 && symTable.element[j].para == -1) return 0;
            
            if(symTable.element[j].type == 3) return 0;//参数表
            if(symTable.element[j].type == 0)//如果是常量
            {
                isConst = 1;
                return symTable.element[j].value;
            }
            if(symTable.element[j].type == 1)//如果是变量
            {
                if(symTable.element[j].para != -1)
                {
                    isArr = 1;
                }
                return symTable.element[j].address;
            }
        }
        //是不是数组
        if(paraNum == 1 && symTable.element[i].para == -1) return 0;
        if(symTable.element[i].type == 3) return 0;//参数表
        if(symTable.element[i].type == 0)
        {
            isConst = 1;
            return symTable.element[i].value;
        }
        if(symTable.element[i].type == 1)
        {
            if(symTable.element[i].para != -1)
            {
                isArr = 1;
            }
            return symTable.element[i].address;
        }
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