#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 512
/**
 * 符号;网上代码要记录address，但是我现在没遇到需要记录的情况，所以暂且没设置这个变量
 */ 
typedef struct
{
    string name;//符号名字
    int type;//0-const 1-var 2-function 3-para
    int value;//常量的值（字符也以int值存储），如果标识符是一个函数名，用1表示函数类型为int，0为void
    int para;//函数参数个数或者数组大小
} Symbol;
/**
 * 符号表;网上代码有符号表栈顶指针，当前符号表有的分程序总数，分程序索引，但我现在没有这种需求
 */
typedef struct
{
    Symbol element[MAX];
    int length;//长度
    int proIndex[MAX];//分程序的索引（比如一个function内的程序就是分程序），增加这个元素以便于快速搜索元素
    int totalPro;//分程序的总数
} SymbolTable;
 
SymbolTable symTable;

void insertSymTab(string name, int type, int value, int para);
void printSymTab();