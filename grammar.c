#include "util.h"
#include "tokens.h"
#include <stdio.h>
#include <stdlib.h>
#include "symbol.h"

extern YYSTYPE yylval;
extern int symID;

void initLex(char *fname);
void getSym();




/**
 * ＜常量定义＞ ::= int＜标识符＞＝＜整数＞{,＜标识符＞＝＜整数＞}| char＜标识符＞＝＜字符＞{,＜标识符＞＝＜字符＞}
 *分析：定义常量定义的格式，常量定义可以是以int 开始后面接一个或者多个顺序如 标识符、=、整数 这样的字符串（如果是多个，则中间以 ， 隔开）的整型常量定义，字符型常量定义与整型类似。
 *为了简单，暂时没有考虑正负数的情况 
 *样例：
 *int a = 1,b = 2,c = 3
 *char a = ‘1’,b = ‘2’,c = ‘3’
 *type为INTSYM or CHARSYM
*/
void constDefine(int type)
{
    string symIDName;
    getSym();
    if(symID == IDSYM)
    {
        symIDName = String(yylval.sval);
        getSym();
        if(symID == ASSIGNSYM)
        {
            getSym();
            if(type == INTSYM && symID == DIGSYM)
            {
                insertSymTab(symIDName, 0, yylval.ival, -1);
            }
            else if(type == CHARSYM && symID == LETTERSYM)
            {
                insertSymTab(symIDName, 0, yylval.sval[1], -1);
            }
            else
            {
                fprintf(stderr,"const defination error!\n"); 
                exit(1);
            }
        }
        else
        {
            fprintf(stderr,"const defination: assign miss error!\n"); 
            exit(1);
        }  
    }
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr,"usage: a.out filename\n"); 
        exit(1);
    }
    initLex(argv[1]);
    getSym();
    constDefine(symID);
    printSymTab();
    //getsym();
    //printf("%d\n",symID);
    return 0;
}