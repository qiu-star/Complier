#include"grammar.h"

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
                    insertIntIntoFourVarCodeTab("const","int",yylval.ival,symIDName);
                }
                else if(type == CHARSYM && symID == LETTERSYM)
                {
                    address++;
                    insertSymTab(symIDName, 0, yylval.sval[1], -1, address);
                    insertStringIntoFourVarCodeTab("const","char",yylval.sval,symIDName);
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
                    if(type == INTSYM) insertIntIntoFourVarCodeTab("inta","",para,symIDName);
                    else if(type == CHARSYM) insertIntIntoFourVarCodeTab("chara","",para,symIDName);
                }
                else
                {
                    address++;
                    para = -1;
                    insertSymTab(symIDName, 1, type, para, address);
                    if(type == INTSYM) insertStringIntoFourVarCodeTab("int","","",symIDName);
                    else if(type == CHARSYM) insertStringIntoFourVarCodeTab("char","","",symIDName);
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
    }while (type == INTSYM || type == CHARSYM);
}

 /**
  * ＜因子＞ ::= ＜标识符＞｜＜标识符＞‘[’＜表达式＞‘]’｜＜整数＞|＜字符＞｜＜有返回值函数调用语句＞|‘(’＜表达式＞‘)’
  * 分析：描述因子的格式，因子是以标识符或者标识符[表达式]或者整数或者字符或者有返回值函数的调用语句或者（表达式）
  * 样例：
  * a[2] 
  * num 
  * b 
  * (a-b+c*d)
  * @return 因子在符号表的名称（字符串）
  */
string factor()
{
    string symIDName;
    string res;
    //如果是标识符
    //＜因子＞ ::= ＜标识符＞
    //＜因子＞ ::= ＜标识符＞‘[’＜表达式＞‘]’
    //＜因子＞ ::= ＜有返回值函数调用语句＞
    if(symID == IDSYM)
    {
        symIDName = String(yylval.sval);
        getSym();
        if(symID == LMPARENSYM)//'[' 数组
        {
            getSym();
            string exprName = expression();//表达式
            if(symID != RMPARENSYM)//']'
            {
                fprintf(stderr,"factor: miss ']' error!\n"); 
                exit(1);
            }
            //调用之前先检查在符号表里是否有该式子
            if(!searchSymTab(symIDName,0,1))
            {
                fprintf(stderr,"symTable don't have %s!\n",symIDName); 
                exit(1);
            }
            //a=b[i];   =>  aassi,b,i,a //array赋值
            res = generateVar();
            insertStringIntoFourVarCodeTab("aassi",symIDName,exprName,res);
            getSym();
            return res;
        }
        else if(symID == LPARENSYM)//'(' 函数调用
        {
            getSym();
            int paraNum = valuePara();//值参数表
            if(symID != RPARENSYM)
            {
                fprintf(stderr,"factor: miss ')' error!\n"); 
                exit(1);
            }
            //调用之前先检查在符号表里是否有该式子
            if(!searchSymTab(symIDName,1,paraNum))
            {
                fprintf(stderr,"call function %s error!\n",symIDName); 
                exit(1);
            }
            //a = f();//有返回值函数调用    =>	call,f,,a
            res = generateVar();
            insertStringIntoFourVarCodeTab("call",symIDName,"",res);
            getSym();
            return res;
        }
        else//标识符
        {
            int t = searchSymTab(symIDName, 0, 0);
            if(t == 0)
            {
                fprintf(stderr,"symTable don't have %s!\n"); 
                exit(1);
            }
            if(isConst)
            {
                return itoa(t);//直接将const值返回
                isConst = 0;
            }
            return symIDName;
        }
    }
    else if(symID == LPARENSYM)//＜因子＞ ::= ‘(’＜表达式＞‘)’
    {
        getSym();
        string exprName = expression();
        if(symID != RPARENSYM)
        {
            fprintf(stderr,"factor: miss ')' error!\n"); 
            exit(1);
        }
        getSym();
        return exprName;
    }
    else if(symID == LETTERSYM)//＜因子＞ ::= ＜字符＞
    {
        res = yylval.sval;
        getSym();
        return res;
    }
    else if(symID == DIGSYM)//＜因子＞ ::= ＜整数＞（无符号
    {
        res = itoa(yylval.ival);
        getSym();
        return res;
    }
    else if(symID == PLUSSYM || symID == MINUSSYM)//＜因子＞ ::= ＜整数＞（有符号
    {
        int sign = (symID == PLUSSYM)? 1: -1;
        getSym();
        if(symID != DIGSYM)
        {
            fprintf(stderr,"factor error: expect a digital number!\n"); 
            exit(1);
        }
        res = itoa(sign * yylval.ival);
        return res;
    }
    else
    {
        fprintf(stderr,"factor error!\n"); 
        exit(1);
    }
    
}

/**
 * ＜项＞ ::= ＜因子＞{＜乘法运算符＞＜因子＞}
 * 分析：描述项的组成格式，以因子开始，后面接若干个 乘法运算符 因子 的组合
 * 样例：a a*b
 */ 
void item()
{

}

/**
 * ＜表达式＞ ::= ［＋｜－］＜项＞{＜加法运算符＞＜项＞}
 * 分析：描述表达式的格式，以项开始，后接若干个 加法运算符 项 的组合，注意，第一个项前面可能有正负号，如果有，则只能有一个。
 * 样例：
 * a + b – (a+b)
 * @returns 表达式在符号表的名称（字符串）
 */ 
string expression()
{

}

/**
 * ＜值参数表＞ ::= ＜表达式＞{,＜表达式＞}｜＜空＞
 * 分析：描述值参数表的格式，是若干个表达式或者空，若为多个表达式，则中间以，分割
 * 样例：a,b 
 * @returns 返回参数表的个数
 */ 
int valuePara()
{
    /*
    人工算first集，然后得到判别条件：
    ＜表达式＞    ::= ［＋｜－］＜项＞{＜加法运算符＞＜项＞}
    ＜项＞     ::= ＜因子＞{＜乘法运算符＞＜因子＞}
    ＜因子＞    ::= ＜标识符＞｜＜标识符＞‘[’＜表达式＞‘]’｜＜整数＞|＜字符＞｜＜有返回值函数调用语句＞|‘(’＜表达式＞‘)’
    ＜有返回值函数调用语句＞ ::= ＜标识符＞‘(’＜值参数表＞‘)’
    First(有返回值函数调用语句)={标识符}
    First(因子)={标识符, 整数DIGSYM, 字符LETTERSYM, ( }
    First(项)=First(因子)
    First(表达式)={+, -, 标识符, 整数DIGSYM, 字符LETTERSYM, ( }
    最终得到结果：First(值参数表)=First(表达式)
    */

}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr,"usage: a.out filename\n"); 
        exit(1);
    }
    initFourVarCodeTab();
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
    printFourVarCodeTab();
    //getsym();
    //printf("%d\n",symID);
    return 0;
}