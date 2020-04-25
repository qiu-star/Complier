#include"grammar.h"

extern YYSTYPE yylval;
extern int symID;

int address;
int hasRet = 0;
int isMain = 0;//是否main函数

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
                    //fprintf(stderr,"var defination: symbol miss error %d!\n",symID); 
                    //exit(1);
                    return;
                }
            }
            else
            {
                fprintf(stderr,"var defination: id miss error! %d\n",symID); 
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
    while (type == INTSYM || type == CHARSYM)
    {
        varDefine(type);
        getSym();
        type = symID;
    }
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
        fprintf(stderr,"factor error !\n"); 
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

void states()
{
    do
    {
        state();
    } while (symID == IFSYM || symID == DOSYM || symID == FORSYM 
        || symID == LBPARENSYM || symID == IDSYM || symID == SCANFSYM
        || symID == PRINTFSYM || symID == RETURNSYM || symID == SEMICSYM);
    
}

/**
 * ＜语句＞ ::= ＜条件语句＞｜＜循环语句＞｜‘{’＜语句列＞‘}’｜＜有返回值函数调用语句＞;|＜无返回值函数调用语句＞;｜＜赋值语句＞;｜＜读语句＞;｜＜写语句＞;｜＜空＞;｜＜返回语句＞;
 * 分析：描述语句的格式，语句是条件语句或者循环语句或者｛语句列｝或者有返回值函数调用语句；或者无返回值函数调用语句；或者赋值语句；或者读语句；或者写语句；或者空；或者返回语句；
 */ 
void state()
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
             * 样例：a = b
             */
            if(searchSymTab(symIDName,0,-1) == 0)
            {
                fprintf(stderr,"undeclare %s!\n",symIDName); 
                exit(1);
            } 
            if(getIsConst())//常量不能赋值
            {
                fprintf(stderr,"for: %s is const, but want a var!\n",symIDName);
                exit(1);
            }
            getSym();
            tmp = expression();
            if(symID != SEMICSYM)//;
            {
                fprintf(stderr,"state: expect ';' !\n");
                exit(1);
            }
            //a=b   =>  =,b,,a
            insertStringIntoFourVarCodeTab("=",tmp,"",symIDName);
            getSym();
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
                fprintf(stderr,"state: expect ';' !\n");
                exit(1);
            }
            getSym();
        }
        else if(symID == SEMICSYM)//;
        {
            getSym();
        }
        else if(symID == LMPARENSYM)//[
        {
            getSym();
            string e = expression();
            if(symID != RMPARENSYM)//]
            {
                fprintf(stderr,"expect ']' !\n");
                exit(1);
            }
            getSym();
            if(symID == ASSIGNSYM)
            {
                /**
                 * ＜赋值语句＞ ::= ＜标识符＞＝＜表达式＞|＜标识符＞‘[’＜表达式＞‘]’=＜表达式＞
                 * 分析：描述赋值语句的格式，是 标识符=表达式 或者 标识符[表达式] = 表达式的形式
                 * 样例：a[m-1] = c + d
                 */
                if(searchSymTab(symIDName,0,1) == 0)
                {
                    fprintf(stderr,"undeclare %s!\n",symIDName); 
                    exit(1);
                } 
                if(getIsConst())//常量不能赋值
                {
                    fprintf(stderr,"for: %s is const, but want a var!\n",symIDName);
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
                insertStringIntoFourVarCodeTab("[]=",tmp,e,symIDName);
                getSym();
            }
        }
        else
        {
            fprintf(stderr,"state error %d!\n",symID);
            exit(1);
        }
        
    }
    else if(symID == SCANFSYM)//＜读语句＞;
    {
        scanfState();
        if(symID != SEMICSYM)//;
        {
            fprintf(stderr,"state: expect ';' !\n");
            exit(1);
        }
        getSym();
    }
    else if(symID == PRINTFSYM)//＜写语句＞;
    {
        printState();
        if(symID != SEMICSYM)//;
        {
            fprintf(stderr,"state: expect ';' !\n");
            exit(1);
        }
        getSym();
    }
    else if(symID == RETURNSYM)//＜返回语句＞;
    {
        returnState();
        if(symID != SEMICSYM)//;
        {
            fprintf(stderr,"state: expect ';' !\n");
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
    string label1 = generateLabel();
    string label2 = generateLabel();
    if(symID == IFSYM)
    {
        getSym();
        if(symID != LPARENSYM)// (
        {
            fprintf(stderr,"expect '(' !\n");
            exit(1);
        }
        getSym();
        condition(0);
        //等于这个条件，直接往下执行，不等于这个条件跳转到else判断（label1）
        insertStringIntoFourVarCodeTab("jne","","",label1);
        if(symID != RPARENSYM)// )
        {
            fprintf(stderr,"expect ')' !\n");
            exit(1);
        }
        getSym();
        //如果满足if语句，直接执行state，state执行结束后跳过else（label2）
        state();
        insertStringIntoFourVarCodeTab("jmp","","",label2);

        //将label1插在else前
        insertStringIntoFourVarCodeTab("lab:","","",label1);
        if(symID == ELSESYM)
        {
            getSym();
            state();
        }
        //将labek插在else结束后
        insertStringIntoFourVarCodeTab("lab:","","",label2);
    }
}

/**
 * ＜条件＞ ::= ＜表达式＞＜关系运算符＞＜表达式＞｜＜表达式＞ //表达式为0条件为假，否则为真
 * 分析：描述条件的格式，条件为 表达式-关系运算符-表达式 或者 表达式 的格式，若表达式为0则认为假，否则认为真，注意关系运算符只含有>、>=等
 * 样例：a>b a 
 * @param inFor 该条件表达式是否用于for循环
 */ 
void condition(int inFor)
{
    string e1 = expression();
    string e2;
    if(symID == SMALLTHSYM)//<
    {
        getSym();
        e2 = expression();
        insertStringIntoFourVarCodeTab("<",e1,e2,"");
    }
    else if(symID == NOTBTHSYM)//<=
    {
        getSym();
        e2 = expression();
        insertStringIntoFourVarCodeTab("<=",e1,e2,"");
    }
    else if(symID == BIGTHSYM)//>
    {
        getSym();
        e2 = expression();
        insertStringIntoFourVarCodeTab(">",e1,e2,"");
    }
    else if(symID == NOTSTHSYM)//>=
    {
        getSym();
        e2 = expression();
        insertStringIntoFourVarCodeTab(">=",e1,e2,"");
    }
    else if(symID == EQUSYM)//==
    {
        getSym();
        e2 = expression();
        insertStringIntoFourVarCodeTab("==",e1,e2,"");
    }
    else if(symID == NOTESYM)//!=
    {
        getSym();
        e2 = expression();
        insertStringIntoFourVarCodeTab("!=",e1,e2,"");
    }
    else if(symID == RPARENSYM)//＜条件＞ ::= ＜表达式＞
    {
        insertStringIntoFourVarCodeTab("!=",e1,"0","");
    }
    else if(inFor && symID == SEMICSYM)//for‘(’＜标识符＞＝＜表达式＞;＜条件＞;＜标识符＞＝＜标识符＞(+|-)
    {
        insertStringIntoFourVarCodeTab("!=",e1,"0","");
    }
    else
    {
        fprintf(stderr,"condition error!\n");
        exit(1);
    }
    
    
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
    string again = generateLabel();
    string out = generateLabel();
    if(symID == DOSYM)
    {
        insertStringIntoFourVarCodeTab("lab:","","",again);//在这设置一个标签，每次返回到这个标签
        getSym();
        state();
        if(symID != WHILESYM)
        {
            fprintf(stderr,"while: expect 'while' !\n");
            exit(1);
        }

        getSym();
        if(symID != LPARENSYM)
        {
            fprintf(stderr,"while: expect '(' !\n");
            exit(1);
        }
        getSym();
        condition(0);
        if(symID != RPARENSYM)
        {
            fprintf(stderr,"while: expect ')' !\n");
            exit(1);
        }
        //如果符合上述条件，跳转到again，如果不符合，跳转到out
        insertStringIntoFourVarCodeTab("jne","","",out);
        insertStringIntoFourVarCodeTab("jmp","","",again);
        insertStringIntoFourVarCodeTab("lab:","","",out);
        getSym();
    }
    else if(symID == FORSYM)
    {
        getSym();
        if(symID != LPARENSYM)
        {
            fprintf(stderr,"for: expect '(' !\n");
            exit(1);
        }
        getSym();
        if(symID != IDSYM)
        {
            fprintf(stderr,"for: expect id !\n");
            exit(1);
        }
        string s1 = yylval.sval;
        if(searchSymTab(s1,0,0) == 0)
        {
            fprintf(stderr,"for: %s undefine!\n",s1);
            exit(1);
        }
        if(getIsConst())//常量也不能赋值
        {
            fprintf(stderr,"for: %s is const, but want a var!\n",s1);
            exit(1);
        }
        getSym();
        if(symID != ASSIGNSYM)
        {
            fprintf(stderr,"for: expect '=' !\n");
            exit(1);
        }
        getSym();
        string e = expression();
        insertStringIntoFourVarCodeTab("=",e,"",s1);
        if(symID != SEMICSYM)
        {
            fprintf(stderr,"for: expect ';' !\n");
            exit(1);
        }
        getSym();

        insertStringIntoFourVarCodeTab("lab:","","",again);
        condition(1);
        //如果不符合条件，则跳出
        insertStringIntoFourVarCodeTab("jne","","",out);
        if(symID != SEMICSYM)
        {
            fprintf(stderr,"for: expect ';' !\n");
            exit(1);
        }
        getSym();
        if(symID != IDSYM)
        {
            fprintf(stderr,"for: expect id !\n");
            exit(1);
        }
        string s2 = yylval.sval;
        if(searchSymTab(s2,0,0) == 0)
        {
            fprintf(stderr,"for: %s undefine!\n",s2);
            exit(1);
        }
        if(getIsConst())//常量也不能赋值
        {
            fprintf(stderr,"for: %s is const, but want a var!\n",s2);
            exit(1);
        }
        getSym();
        if(symID != ASSIGNSYM)
        {
            fprintf(stderr,"for: expect '=' !\n");
            exit(1);
        }
        getSym();
        if(symID != IDSYM)
        {
            fprintf(stderr,"for: expect id !\n");
            exit(1);
        }
        string s3 = yylval.sval;
        if(searchSymTab(s3,0,0) == 0)
        {
            fprintf(stderr,"for: %s undefine!\n",s3);
            exit(1);
        }
        if(getIsConst())//常量也不能赋值
        {
            fprintf(stderr,"for: %s is const, but want a var!\n",s3);
            exit(1);
        }
        getSym();
        if(symID != PLUSSYM && symID != MINUSSYM)
        {
            fprintf(stderr,"for: expect +/- !\n");
            exit(1);
        }
        string op;
        if(symID == PLUSSYM)
        {
            op = String("+");
        }
        else
        {
            op = String("-");
        }
        getSym();
        if(symID != DIGSYM)
        {
            fprintf(stderr,"for: expect a num !\n");
            exit(1);
        }
        string tmp = generateVar();
        insertIntIntoFourVarCodeTab(op,s3,yylval.ival,tmp);
        insertStringIntoFourVarCodeTab("=",tmp,"",s2);
        
        getSym();
        if(symID != RPARENSYM)
        {
            fprintf(stderr,"for: expect ')' !\n");
            exit(1);
        }

        getSym();
        state();

        insertStringIntoFourVarCodeTab("jmp","","",again);
        insertStringIntoFourVarCodeTab("lab:","","",out);
    }
    else
    {
        fprintf(stderr,"loop error!\n");
        exit(1);
    }
}


/**
 * ＜读语句＞ ::= scanf ‘(’＜标识符＞{,＜标识符＞}‘)’
 * 分析：描述读语句的格式，sacnf（若干个标识符（至少一个））
 * 样例：scanf(a,b,c)
 */ 
void scanfState()
{
    if(symID != SCANFSYM)
    {
        fprintf(stderr,"scanf: expect 'scanf' !\n");
        exit(1);
    }
    getSym();
    if(symID != LPARENSYM)
    {
        fprintf(stderr,"scanf: expect '(' !\n");
        exit(1);
    }
    getSym();
    if(symID != IDSYM)
    {
        fprintf(stderr,"scanf: expect a id!\n");
        exit(1);
    }
    if(searchSymTab(yylval.sval,0,0) == 0)
    {
        fprintf(stderr,"scanf: '%s' isn't a var/ undefine !\n",yylval.sval);
        exit(1);
    }
    if(getIsArr())
    {
        fprintf(stderr,"scanf: '%s' is a array !\n",yylval.sval);
        exit(1);
    }

    //scanf(a); =>  scf,,a的type:char/int,a
    string type = (getFactorType() == INTSYM)?"int":"char";
    insertStringIntoFourVarCodeTab("scf","",type,yylval.sval);
    getSym();
    while(symID == COMMASYM)
    {
        getSym();
        if(symID != IDSYM)
        {
            fprintf(stderr,"scanf: expect a id!\n");
            exit(1);
        }
        if(searchSymTab(yylval.sval,0,0) == 0)
        {
            fprintf(stderr,"scanf: '%s' isn't a var/ undefine !\n",yylval.sval);
            exit(1);
        }
        if(getIsArr())
        {
            fprintf(stderr,"scanf: '%s' is a array !\n",yylval.sval);
            exit(1);
        }
        type = (getFactorType() == INTSYM)?"int":"char";
        insertStringIntoFourVarCodeTab("scf","",type,yylval.sval);
        getSym();
    }
    if(symID != RPARENSYM)
    {
        fprintf(stderr,"scanf: expect ')' !\n");
        exit(1);
    }
    getSym();
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
    if(symID != PRINTFSYM)
    {
        fprintf(stderr,"printf: expect 'printf' !\n");
        exit(1);
    }
    getSym();
    if(symID != LPARENSYM)
    {
        fprintf(stderr,"printf: expect '(' !\n");
        exit(1);
    }
    getSym();
    if(symID == STRINGSYM)
    {
        string t = yylval.sval;
        getSym();
        if(symID == RPARENSYM)
        {
            //printf ‘(’＜字符串＞‘)’
            insertStringIntoFourVarCodeTab("prtf",t,"","char");
            getSym();
        }
        else if(symID == COMMASYM)
        {
            getSym();
            string e = expression();
            //这个地方可能有问题
            string type = (getFactorType() == INTSYM)?"int":"char";
            insertStringIntoFourVarCodeTab("prtf",t,e,type);
            if(symID != RPARENSYM)
            {
                fprintf(stderr,"printf: expect ')' !\n");
                exit(1);
            }
            getSym();
        }
        else
        {
            fprintf(stderr,"printf: error !\n");
            exit(1);
        }
    }
    else
    {
        string e = expression();
        if(symID != RPARENSYM)
        {
            fprintf(stderr,"printf: expect ')' !\n");
            exit(1);
        }
        string type = (getFactorType() == INTSYM)?"int":"char";
        insertStringIntoFourVarCodeTab("prtf","",e,type);
        getSym();
    }
    
}

/**
 * ＜返回语句＞ ::= return[‘(’＜表达式＞‘)’]
 * 分析：描述返回语句的格式，return或者return（表达式）
 * 样例：
 * return
 * return (a) 
 */ 
void returnState()
{
    if(symID != RETURNSYM)
    {
        fprintf(stderr,"return: expect 'return'!\n"); 
        exit(1);
    }
    getSym();
    if(symID == LPARENSYM)
    {
        getSym();
        string e = expression();
        insertStringIntoFourVarCodeTab("ret","","",e);
        if(symID != RPARENSYM)
        {
            fprintf(stderr,"return: expect ')'!\n"); 
            exit(1);
        }
        hasRet = 1;
        getSym();
    }
    else if(symID == SEMICSYM)
    {
        if(isMain) insertStringIntoFourVarCodeTab("exit","","","");
        else insertStringIntoFourVarCodeTab("ret","","","");
    }
}

/**
 * ＜程序＞ ::= ［＜常量说明＞］［＜变量说明＞］{＜有返回值函数定义＞|＜无返回值函数定义＞}＜主函数＞
 * 分析：程序定义为严格顺序的 常量说明-字符串说明-有返回值函数定义-无返回值函数定义-主函数 的字符串组合，其中常量说明和变量说明可以没有，如果有，只能出现一次（即不存在多个常量或者变量说明语句），有返回值函数定义和无返回值函数定义可以有若干个，以主程序结尾。
 * 样例：
 * const int a，b，c；
 * int q，w，e；
 * void func1（）｛
 * …
 * ｝
 * Int func2（）｛
 * …
 * ｝
 * Void main(){
 * …
 * }
 */ 
void programme()
{
    if(symID == CONSTSYM)constDeclare();
    
    //不能直接varDeclare因为不确定是变量头还是函数头
    int type;
    string symIDName;
    while(symID == INTSYM || symID == CHARSYM)
    {
        type = symID;
        getSym();
        if(symID != IDSYM)
        {
            fprintf(stderr,"programme: expect 'id' !\n");
            exit(1);
        }
        symIDName = yylval.sval;
        getSym();
        if(symID == LMPARENSYM)//如果是[]说明一定是变量定义
        {
            getSym();
            if(symID != DIGSYM)
            {
                fprintf(stderr,"var defination: index miss error!\n"); 
                exit(1);
            }
            int para = yylval.ival;
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
            if(symID == COMMASYM)
            {
                varDefine(type);
            }
            if(symID != SEMICSYM)
            {
                fprintf(stderr,"expect ';' !\n");
                exit(1);
            }
            getSym();
        }
        else if(symID == COMMASYM)//如果是逗号，可以确定是变量
        {
            address++;
            int para = -1;
            insertSymTab(symIDName, 1, type, para, address);
            if(type == INTSYM) insertStringIntoFourVarCodeTab("int","","",symIDName);
            else if(type == CHARSYM) insertStringIntoFourVarCodeTab("char","","",symIDName);
            varDefine(type);
            if(symID != SEMICSYM)
            {
                fprintf(stderr,"expect ';' !\n");
                exit(1);
            }
            getSym();
        }
        else if(symID == SEMICSYM)
        {
            address++;
            int para = -1;
            insertSymTab(symIDName, 1, type, para, address);
            if(type == INTSYM) insertStringIntoFourVarCodeTab("int","","",symIDName);
            else if(type == CHARSYM) insertStringIntoFourVarCodeTab("char","","",symIDName);
            getSym();
        }
        else if(symID == LPARENSYM || symID == VOIDSYM)
        {
            break;
        }
        else
        {
            fprintf(stderr,"programme: error!");
            exit(1);
        }
    }
    if(symID == LPARENSYM)//‘(’说明是函数
    {
        address = 0;
        insertSymTab(symIDName,2,type,0,address);//后续会通过insertPara将参数个数更新
        if(type == INTSYM) insertStringIntoFourVarCodeTab("func","int","",yylval.sval);
        else if(type == CHARSYM) insertStringIntoFourVarCodeTab("func","char","",yylval.sval);
        funcWithRetDefine(0,symIDName);
    }
    while(symID == INTSYM || symID == CHARSYM || symID == VOIDSYM)
    {
        if(symID == INTSYM || symID == CHARSYM)
        {
            funcWithRetDefine(1,NULL);
        }
        else if(symID == VOIDSYM)
        {
            funcWithoutRetDefine();
        }
        if(isMain)break;
    }
    
}

/**
 * ＜声明头部＞ ::= int＜标识符＞|char＜标识符＞
 * 分析：定义声明头部的格式，以int或char开头的后接标识符的字符串
 * 样例：int a char b
 * @return 头部名称
 */ 
string declareHead()
{
    int type;
    if(symID == INTSYM || symID == CHARSYM)
    {
        type = symID;
        address = 0;
        getSym();
        if(symID != IDSYM)
        {
            fprintf(stderr,"declareHead: expect 'id'!\n");
            exit(1);
        }
        string symIDName = yylval.sval;
        insertSymTab(symIDName,2,type,0,address);//后续会通过insertPara将参数个数更新
        if(type == INTSYM) insertStringIntoFourVarCodeTab("func","int","",yylval.sval);
        else if(type == CHARSYM) insertStringIntoFourVarCodeTab("func","char","",yylval.sval);
        getSym();
        return symIDName;
    }
    else
    {
        fprintf(stderr,"declareHead: expect 'int'/'char'!\n");
        exit(1);
    }
}

/**
 * ＜有返回值函数定义＞ ::= ＜声明头部＞‘(’＜参数＞‘)’ ‘{’＜复合语句＞‘}’
 * 分析：描述有返回值函数定义的格式，包含声明头部，被（）的参数，被｛｝包裹的复合语句，三者严格有序
 * 样例：
 * int cmp(int a, int b){
 * …
 * }
 * @param needDeclare 是否需要声明函数头
 * @param name 如果不需要声明函数头，那么name为函数名，否则为null
 */
void funcWithRetDefine(int needDeclare, string name)
{
    string funcName;
    if(needDeclare)
    {
        hasRet = 0;
        funcName = declareHead();
    }
    else
    {
        funcName = name;
    }
    
    
    if (symID != LPARENSYM)
    {
        fprintf(stderr,"funcWithRetDefine: expect '(' !\n");
        exit(1);
    }
    getSym();
    paraTable();
    if(symID != RPARENSYM)
    {
        fprintf(stderr,"funcWithRetDefine: expect ')' !\n");
        exit(1);
    }
    getSym();
    if(symID != LBPARENSYM)
    {
        fprintf(stderr,"funcWithRetDefine: expect '{' !\n");
        exit(1);
    }
    getSym();
    compoundState();
    if(symID != RBPARENSYM)
    {
        fprintf(stderr,"funcWithRetDefine: expect '}' !\n");
        exit(1);
    }
    if(hasRet == 0)
    {
        fprintf(stderr,"funcWithRetDefine: expect a return value !\n");
        exit(1);
    }
    getSym();
    insertStringIntoFourVarCodeTab("end","","",funcName);
    hasRet = 0;
}

/**
 * ＜无返回值函数定义＞ ::= void＜标识符＞‘(’＜参数＞‘)’‘{’＜复合语句＞‘}’
 * 分析：描述无返回值函数定义的格式，以void开始，被（）包裹的参数，被｛｝包裹的复合语句，三者严格有序
 * 样例：
 * void cmp(int a, int b){
 * …
 * } 
 */ 
void funcWithoutRetDefine()
{
    hasRet = 0;
    if(symID != VOIDSYM)
    {
        fprintf(stderr,"funcWithoutRetDefine: expect 'void' !\n");
        exit(1);
    }
    getSym();
    if(symID == MAINSYM)
    {
        if(isMain)//说明之前有main
        {
            fprintf(stderr,"must have only one main func!\n");
            exit(1);
        }
        mainFunc();
        return;
    }
    if(symID != IDSYM)
    {
        fprintf(stderr,"funcWithoutRetDefine: expect 'id' !\n");
        exit(1);
    }
    address = 0;
    string funcName = yylval.sval;
    insertSymTab(funcName,2,0,0,address);
    insertStringIntoFourVarCodeTab("func","void","",funcName);
    getSym();
    if(symID != LPARENSYM)
    {
        fprintf(stderr,"funcWithoutRetDefine: expect '(' !\n");
        exit(1);
    }
    getSym();
    paraTable();
    if(symID != RPARENSYM)
    {
        fprintf(stderr,"funcWithoutRetDefine: expect ')' !\n");
        exit(1);
    }
    getSym();
    if(symID != LBPARENSYM)
    {
        fprintf(stderr,"funcWithoutRetDefine: expect '{' !\n");
        exit(1);
    }
    getSym();
    compoundState();
    if(symID != RBPARENSYM)
    {
        fprintf(stderr,"funcWithoutRetDefine: expect '}' !\n");
        exit(1);
    }
    if(hasRet)
    {
        fprintf(stderr,"funcWithoutRetDefine: shouldn't return value !\n");
        exit(1);
    }
    getSym();
    insertStringIntoFourVarCodeTab("end","","",funcName);
}

/**
 * ＜复合语句＞ ::= ［＜常量说明＞］［＜变量说明＞］＜语句列＞
 * 分析：描述复合语句的格式，由常量说明、变量说明、语句列组成，三者严格有序，其中常量说明，变量说明可有可无，若存在，顺序不可逆
 * 样例：
 * const int a =1 ;
 * int b;
 * b = 4;
 */
void compoundState()
{
    if(symID == CONSTSYM)
    {
        constDeclare();
    }
    if(symID == INTSYM || symID == CHARSYM)
    {
        varDeclare(symID);
    }
    states();
}

/**
 * ＜参数＞ ::= ＜参数表＞
 * ＜参数表＞ ::= ＜类型标识符＞＜标识符＞{,＜类型标识符＞＜标识符＞}| ＜空＞
 * 分析：描述参数表的格式，由一个或者多个 类型标识符接标识符 的形式组成，若有多个，则中间以，分割，参数表可以为空，参数表中不能出现数组
 * 样例：
 * int a,int b, int c
 */ 
void paraTable()
{
    int paramNum = 0;
    int paratype;
    if(symID == INTSYM || symID == CHARSYM)
    {
        paratype = symID;
        getSym();
        if(symID != IDSYM)
        {
            fprintf(stderr,"paraTable: expect 'id' !\n");
            exit(1);
        }
        address++;
        insertSymTab(yylval.sval,3,paratype,paramNum+1,address);
        if(paratype == INTSYM) insertStringIntoFourVarCodeTab("para","int","",yylval.sval);
        else if(paratype == CHARSYM) insertStringIntoFourVarCodeTab("para","char","",yylval.sval);
        paramNum++;
        getSym();
        while(symID == COMMASYM)
        {
            getSym();
            if(symID == INTSYM || symID == CHARSYM)
            {
                paratype = symID;
                getSym();
                if(symID != IDSYM)
                {
                    fprintf(stderr,"paraTable: expect 'id' !\n");
                    exit(1);
                }
                address++;
                insertSymTab(yylval.sval,3,paratype,paramNum+1,address);
                if(paratype == INTSYM) insertStringIntoFourVarCodeTab("para","int","",yylval.sval);
                else if(paratype == CHARSYM) insertStringIntoFourVarCodeTab("para","char","",yylval.sval);
                paramNum++;
                getSym();
            }
            else
            {
                fprintf(stderr,"paraTable error!\n");
                exit(1);
            }
        }
        insertPara(paramNum);
    }
}

/**
 * ＜主函数＞ ::= void main‘(’‘)’ ‘{’＜复合语句＞‘}’
 * 分析：描述主函数的格式，以 void main（）开始，后接被｛｝包裹的复合语句
 * 样例：
 * void main(){
 * …
 * }
 */ 
void mainFunc()
{
    address = 0;
    insertSymTab("main",2,0,address,0);
    insertStringIntoFourVarCodeTab("func","","","main");
    isMain = 1;
    getSym();
    if(symID != LPARENSYM)
    {
        fprintf(stderr,"main: need a '(' !\n");
        exit(1);
    }
    getSym();
    if(symID != RPARENSYM)
    {
        fprintf(stderr,"main: need a ')' !\n");
        exit(1);
    }
    getSym();
    if(symID != LBPARENSYM)
    {
        fprintf(stderr,"main: need a '{' !\n");
        exit(1);
    }
    compoundState();
    insertStringIntoFourVarCodeTab("end","","","main");
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
    programme();
    printSymTab();
    printFourVarCodeTab();
    //getsym();
    //printf("%d\n",symID);
    return 0;
}