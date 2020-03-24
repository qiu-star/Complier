typedef union
{
    int ival;
    string sval;
    float fval;
    char cval;
} YYSTYPE;
extern YYSTYPE yylval;

/**
 * C语言出现冒号是什么情况,
 * long short型是否弄，
 * BOUND是否有落下的, 
 * string型关键字需要定义吗
 * double没有定义（如果定义的话需要和float分开
 * unsigned、signed没有定义
 * ?没有定义
 */ 
enum TYPE{
    ERROR,  //错误
    STRING,    //字符串（含内容）
    INT,    //int数
    CHAR,   //char字符
    FLOAT,  //float数
    BOUND,  //界限符+ - * / && || & | >= <= > < == != ( ) { } [] ; , =
    ID,
    KW_INT, //int关键字
    KW_CHAR,//char关键字
    KW_AUTO,//auto关键字
    KW_FLOAT,//float关键字
    KW_STRUCT,//struct关键字
    KW_UNION,//union关键字
    KW_ENUM,
    KW_TYPEDEF,
    KW_CONST,
    KW_EXTERN,
    KW_REGISTER,
    KW_STATIC,
    KW_VOLATILE,
    KW_VOID,
    KW_ELSE,
    KW_SWITCH,
    KW_CASE,
    KW_FOR,
    KW_DO,
    KW_WHILE,
    KW_GOTO,
    KW_CONTINUE,
    KW_BREAK,
    KW_DEFAULT,
    KW_SIZEOF,
    KW_RETURN,
    KW_IF,
    KW_PRINTF,
    KW_SCANF,
    END
}; 
