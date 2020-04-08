#include "util.h"
#include "tokens.h"
#include <stdio.h>
#include <stdlib.h>
#include "symbol.h"

extern YYSTYPE yylval;
extern int symID;

int address;

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
    do
    {
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
                    address++;
                    insertSymTab(symIDName, 0, yylval.ival, -1, address);
                }
                else if(type == CHARSYM && symID == LETTERSYM)
                {
                    address++;
                    insertSymTab(symIDName, 0, yylval.sval[1], -1, address);
                }
                else
                {
                    fprintf(stderr,"const defination error!\n"); 
                    exit(1);
                }

                getSym();
                if(symID == SEMICSYM)// ;
                {
                    return;
                }
                else if(symID != COMMASYM)//不为','
                {
                    fprintf(stderr,"const defination: symbol miss error!\n"); 
                    exit(1);
                }
            }
            else
            {
                fprintf(stderr,"const defination: assign miss error!\n"); 
                exit(1);
            }
    
        }
    }while (1);
    
}

/**
 * ＜常量说明＞ ::= const＜常量定义＞;
 * 分析：定义常量说明的格式，以const开头，后接常量定义，以；结尾，这样形式的字符串可以有多个
 * 样例：
 * const int a = 1 ; 
 * 如果前置是const，调用这个函数
 */
void constDeclare()
{
    do
    {
        getSym();
        constDefine(symID);
        getSym();
    } while (symID == CONSTSYM);
}

/**
 * ＜变量定义＞ ::= ＜类型标识符＞(＜标识符＞|＜标识符＞‘[’＜无符号整数＞‘]’){,(＜标识符＞|＜标识符＞‘[’＜无符号整数＞‘]’) }
 * 分析：描述变量定义的形式，一个或者多个 类型标识符后接标识符或标识符[无符号整数] 的形式，如果有多个，则中间以 ， 联接
 * 样例：
 * int a, b
 * int c[3], b[3]
 */ 
void varDefine(int type)
{
    int para;
    string symIDName;
    if(type == INTSYM || type == CHARSYM)
    {
        do
        {
            para = 0;
            getSym();
            if(symID == IDSYM)
            {
                symIDName = String(yylval.sval);
                getSym();//如果不是'['，就会是','和‘;’
                if(symID == LMPARENSYM)// [
                {
                    getSym();
                    if(symID != DIGSYM)
                    {
                        fprintf(stderr,"var defination: index miss error!\n"); 
                        exit(1);
                    }
                    para = yylval.ival;
                    getSym();
                    if(symID != RMPARENSYM)// ]
                    {
                        fprintf(stderr,"var defination: ']' miss error!\n"); 
                        exit(1);
                    }
                    address += para;
                    insertSymTab(symIDName, 1, type, para, address);
                    getSym();
                }
                else
                {
                    address++;
                    para = -1;
                    insertSymTab(symIDName, 1, type, para, address);
                }

                if(symID == SEMICSYM)// ;
                {
                    return;
                }
                else if(symID != COMMASYM)//不为','
                {
                    fprintf(stderr,"var defination: symbol miss error!\n"); 
                    exit(1);
                }
            }
            else
            {
                fprintf(stderr,"var defination: id miss error!\n"); 
                exit(1);
            }     
        } while (1);
        
    }
}

/**
 * ＜变量说明＞ ::= ＜变量定义＞;{＜变量定义＞;}
 * 分析：定义变量说明语句的格式，是一个或更多的形如 变量定义； 的字符串 
 */
 void varDeclare(int type)
 {
    do
    {
        varDefine(type);
        getSym();
        type = symID;
    } while (type == INTSYM || type == CHARSYM);
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
    if(symID == CONSTSYM)
    {
        constDeclare();
    }
    if(symID == INTSYM || symID == CHARSYM)
    {
        varDeclare(symID);
    }
    printSymTab();
    //getsym();
    //printf("%d\n",symID);
    return 0;
}