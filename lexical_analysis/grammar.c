#include "util.h"
#include "tokens.h"
#include <stdio.h>
#include <stdlib.h>

extern YYSTYPE yylval;
extern int symID;

void init_lex(char *fname);
void getsym();

/**
 * 符号表
 */ 
typedef struct
{
    string name;//符号名字
    int type;//0-const 1-var 2-function 3-para
    int value;//常量的值
};


/**
 * ＜常量定义＞ ::= int＜标识符＞＝＜整数＞{,＜标识符＞＝＜整数＞}| char＜标识符＞＝＜字符＞{,＜标识符＞＝＜字符＞}
 *分析：定义常量定义的格式，常量定义可以是以int 开始后面接一个或者多个顺序如 标识符、=、整数 这样的字符串（如果是多个，则中间以 ， 隔开）的整型常量定义，字符型常量定义与整型类似。
 *为了简单，暂时没有考虑正负数的情况 
 *样例：
 *int a = 1,b = 2,c = 3
 *char a = ‘1’,b = ‘2’,c = ‘3’
 *type为INTSYM or CHARSYM
*/
void const_define(int type)
{
    string symIDName;
    getsym();
    if(symID == IDSYM)
    {
        symIDName = String(yylval.sval);
        getsym();
        if(symID == ASSIGNSYM)
        {
            getsym();
            if(type == INTSYM && symID == DIGSYM)
            {

            }
            else if(type == CHARSYM && symID == LETTERSYM)
            {

            }
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
    init_lex(argv[1]);
    //getsym();
    //printf("%d\n",symID);
    return 0;
}