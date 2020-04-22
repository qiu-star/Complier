#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 512

/**
 * 四元式
 */ 
typedef struct
{
    string op;
    string num_a;//var1
    string num_b;//var2
    string rst;//var3
}FourVarCode;
/**
 * 四元式表
 */ 
typedef struct
{
    FourVarCode element[MAX];
    int length;//记录四元式表中的项数
}FourVarCodeTab;

void initFourVarCodeTab();
void insertIntIntoFourVarCodeTab(char* op, char* a, int b, char* result);
void insertStringIntoFourVarCodeTab(char* op, char* a, char* b, char* result);
string generateVar();
void printFourVarCodeTab();