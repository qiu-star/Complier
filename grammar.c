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
                fprintf(stderr,"undeclare %s!\n",symIDName); 
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
                fprintf(stderr,"undeclare %s!\n",symIDName); 
                exit(1);
            }
            if(getIsConst())
            {
                setIsConst(0);
                return itoa(t);//直接将const值返回
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
        getSym();
        return res;
    }
    else
    {
        fprintf(stderr,"factor error!\n"); 
        exit(1);
    }
    return NULL;
}

/**
 * ＜项＞ ::= ＜因子＞{＜乘法运算符＞＜因子＞}
 * 分析：描述项的组成格式，以因子开始，后面接若干个 乘法运算符 因子 的组合
 * 样例：a a*b
 * @return 
 */ 
string item()
{
    string tmp1, tmp2, tmp3;
    tmp3 = factor();
    while(symID == MULTISYM || symID == DIVSYM)
    {
        tmp1 = tmp3;
        if(symID == MULTISYM)
        {
            getSym();
            tmp2 = factor();
            tmp3 = generateVar();
            insertStringIntoFourVarCodeTab("*",tmp1,tmp2,tmp3);
        }
        else
        {
            getSym();
            tmp2 = factor();
            tmp3 = generateVar();
            insertStringIntoFourVarCodeTab("/",tmp1,tmp2,tmp3);
        }
    }
    return tmp3;
}

/**
 * ＜表达式＞ ::= ［＋｜－］＜项＞{＜加法运算符＞＜项＞}
 * 分析：描述表达式的格式，以项开始，后接若干个 加法运算符 项 的组合，注意，第一个项前面可能有正负号，如果有，则只能有一个。
 * 样例：
 * a + b – (a+b)
 * @return 表达式在符号表的名称（字符串）
 */ 
string expression()
{
    string tmp1, tmp2, tmp3;
    if(symID == PLUSSYM || symID == MINUSSYM)
    {
        if(symID == PLUSSYM)
        {
            getSym();
            tmp3 = item();
        }
        else
        {
            getSym();
            tmp1 = String("0");
            tmp2 = item();
            tmp3 = generateVar();
            insertStringIntoFourVarCodeTab("-",tmp1,tmp2,tmp3);
        }
        while(symID == PLUSSYM || symID == MINUSSYM)
        {
            tmp1 = tmp3;
            if(symID == PLUSSYM)
            {
                getSym();
                tmp2 = item();
                tmp3 = generateVar();
                insertStringIntoFourVarCodeTab("+",tmp1,tmp2,tmp3);
            }
            else
            {
                getSym();
                tmp2 = item();
                tmp3 = generateVar();
                insertStringIntoFourVarCodeTab("-",tmp1,tmp2,tmp3);
            }
        }
    }
    else
    {
        tmp3 = item();
        while (symID == PLUSSYM || symID == MINUSSYM)
        {
            tmp1 = tmp3;
            if(symID == PLUSSYM)
            {
                getSym();
                tmp2 = item();
                tmp3 = generateVar();
                insertStringIntoFourVarCodeTab("+",tmp1,tmp2,tmp3);
            }
            else
            {
                getSym();
                tmp2 = item();
                tmp3 = generateVar();
                insertStringIntoFourVarCodeTab("-",tmp1,tmp2,tmp3);
            }
        }
    }
    return tmp3;
}

/**
 * ＜值参数表＞ ::= ＜表达式＞{,＜表达式＞}｜＜空＞
 * 分析：描述值参数表的格式，是若干个表达式或者空，若为多个表达式，则中间以，分割
 * 样例：a,b 
 * @return 返回参数表的个数
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
   int paraNum = 0;
   string t;
   if (symID == PLUSSYM || symID == MINUSSYM || symID == IDSYM ||
        symID == DIGSYM || symID == LETTERSYM || symID == LPARENSYM)
   {
       t = expression();
       insertStringIntoFourVarCodeTab("cpara","","",t);//值参数 callparam
   }
   while(symID == COMMASYM)//','
   {
       getSym();
       if (symID == PLUSSYM || symID == MINUSSYM || symID == IDSYM ||
        symID == DIGSYM || symID == LETTERSYM || symID == LPARENSYM)
        {
            t = expression();
            insertStringIntoFourVarCodeTab("cpara","","",t);//值参数 callparam
            paraNum++;
        }
   }
   return paraNum;
}

/**
 * ＜语句＞ ::= ＜条件语句＞｜＜循环语句＞｜‘{’＜语句列＞‘}’｜＜有返回值函数调用语句＞;|＜无返回值函数调用语句＞;｜＜赋值语句＞;｜＜读语句＞;｜＜写语句＞;｜＜空＞;｜＜返回语句＞;
 * 分析：描述语句的格式，语句是条件语句或者循环语句或者｛语句列｝或者有返回值函数调用语句；或者无返回值函数调用语句；或者赋值语句；或者读语句；或者写语句；或者空；或者返回语句；
 */ 
void states()
{
    if(symID == IFSYM)//＜条件语句＞
    {
        ifState();
    }
    else if(symID == DOSYM || symID == FORSYM)//＜循环语句＞
    {
        loopState();
    }
    else if(symID == LBPARENSYM)//‘{’＜语句列＞‘}’
    {
        /**
         * ＜语句列＞ ::=｛＜语句＞｝
         * 分析：描述语句列的格式，是包裹在｛｝中的语句
         * 样例：
         * {
         * …
         * }
         */
        getSym();
        states();
        if(symID != RBPARENSYM)
        {
            fprintf(stderr,"states: miss '}'!\n"); 
            exit(1);
        } 
        getSym();
    }
    else if(symID == IDSYM)//＜有返回值函数调用语句＞;|＜无返回值函数调用语句＞;｜＜赋值语句＞;
    {
        string symIDName = yylval.sval;
        string tmp;
        getSym();
        if(symID == ASSIGNSYM)
        {
            /**
             * ＜赋值语句＞ ::= ＜标识符＞＝＜表达式＞|＜标识符＞‘[’＜表达式＞‘]’=＜表达式＞
             * 分析：描述赋值语句的格式，是 标识符=表达式 或者 标识符[表达式] = 表达式的形式
             * 样例：a = b a[m-1] = c + d
             */
            if(searchSymTab(symIDName,0,-1) == 0)
            {
                fprintf(stderr,"undeclare %s!\n",symIDName); 
                exit(1);
            } 
            getSym();
            tmp = expression();
            if(symID != SEMICSYM)//;
            {
                fprintf(stderr,"expect ';' !\n");
                exit(1);
            }
            //a=b   =>  =,b,,a
            insertStringIntoFourVarCodeTab("=",tmp,"",symIDName);
        }
        else if(symID == LPARENSYM)//＜有返回值函数调用语句＞;|＜无返回值函数调用语句＞;
        {
            /**
             * ＜有返回值函数调用语句＞ ::= ＜标识符＞‘(’＜值参数表＞‘)’
             * ＜无返回值函数调用语句＞ ::= ＜标识符＞‘(’＜值参数表＞‘)’
             */ 
            getSym();
            int paramNum = valuePara();
            if(!searchSymTab(symIDName,1,paramNum))
            {
                fprintf(stderr,"states: don't have function %s/ param num doesn't match!\n",symIDName); 
                exit(1);
            }
            if(symID != RPARENSYM)
            {
                fprintf(stderr,"states: miss ')'!\n"); 
                exit(1);
            }
            //f();    =>	call,f,,
            insertStringIntoFourVarCodeTab("call",symIDName,"","");
            getSym();
            if (symID != SEMICSYM)//;
            {
                fprintf(stderr,"expect ';' !\n");
                exit(1);
            }
            getSym();
        }
        else if(symID == SEMICSYM)//;
        {
            getSym();
        }
        else
        {
            fprintf(stderr,"state error!\n");
            exit(1);
        }
        
    }
    else if(symID == SCANFSYM)//＜读语句＞;
    {
        scanfState();
        if(symID != SEMICSYM)//;
        {
            fprintf(stderr,"expect ';' !\n");
            exit(1);
        }
        getSym();
    }
    else if(symID == PRINTFSYM)//＜写语句＞;
    {
        printState();
        if(symID != SEMICSYM)//;
        {
            fprintf(stderr,"expect ';' !\n");
            exit(1);
        }
        getSym();
    }
    else if(symID == RETURNSYM)//＜返回语句＞;
    {
        returnState();
        if(symID != SEMICSYM)//;
        {
            fprintf(stderr,"expect ';' !\n");
            exit(1);
        }
        getSym();
    }
    else if(symID == SEMICSYM)//; 算是空语句的一种
    {
        getSym();
    }
    
}



/**
 * ＜条件语句＞ ::= if ‘(’＜条件＞‘)’＜语句＞［else＜语句＞］
 * 分析：描述条件语句的格式，else 可有可无，若出现仅一次，但是允许嵌套 
 */ 
void ifState()
{

}

/**
 * ＜条件＞ ::= ＜表达式＞＜关系运算符＞＜表达式＞｜＜表达式＞ //表达式为0条件为假，否则为真
 * 分析：描述条件的格式，条件为 表达式-关系运算符-表达式 或者 表达式 的格式，若表达式为0则认为假，否则认为真，注意关系运算符只含有>、>=等
 * 样例：a>b a 
 * @return 返回条件表达式存储变量位置字符串
 */ 
string condition()
{

}

/**
 * ＜循环语句＞ ::= do＜语句＞while ‘(’＜条件＞‘)’ |for‘(’＜标识符＞＝＜表达式＞;＜条件＞;＜标识符＞＝＜标识符＞(+|-)＜步长＞‘)’＜语句＞
 * 分析：描述循环语句的格式，包括 do while 类型的循环，和for循环，其中，do while循环的格式为，do开始的后接 语句 while（条件）的形式，for循环为for开始的后接 （标识符=表达式）；条件；标识符=标识符+（或者-）步长），以语句结尾的形式。
 * 样例：
 * do{
 * …
 * }while(a>1)
 * for(a = 0 ; a < 100 ; a = a +1)｛
 * …
 * }
 */ 
void loopState()
{

}


/**
 * ＜读语句＞ ::= scanf ‘(’＜标识符＞{,＜标识符＞}‘)’
 * 分析：描述读语句的格式，sacnf（若干个标识符（至少一个））
 * 样例：scanf(a,b,c)
 */ 
void scanfState()
{

}

/**
 * ＜写语句＞ ::= printf‘(’＜字符串＞,＜表达式＞‘)’|printf ‘(’＜字符串＞‘)’|printf ‘(’＜表达式＞‘)’
 * 分析：描述写语句的格式，printf(字符串，表达式)或者printf（字符串）或者printf（表达式）
 * 样例：
 * printf(“hello world”)
 * printf(“%d”,a)
 * printf(a)
 */ 
void printState()
{

}

/**
 * ＜返回语句＞ ::= return[‘(’＜表达式＞‘)’]
 * 分析：描述返回语句的格式，return或者return（表达式）
 * 样例：
 * return
 * return a 
 */ 
void returnState()
{

}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr,"usage: a.out filename\n"); 
        exit(1);
    }
    initFourVarCodeTab();
    initSymTab();
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
    valuePara();
    printSymTab();
    printFourVarCodeTab();
    //getsym();
    //printf("%d\n",symID);
    return 0;
}