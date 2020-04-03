typedef union
{
    int ival;
    string sval;
} YYSTYPE;
extern YYSTYPE yylval;


#define INTSYM 1 // int
#define CHARSYM 2 // char
#define CONSTSYM 3 // const
#define MAINSYM 4 // main
#define IFSYM 5 // if
#define ELSESYM 6 // else
#define DOSYM 7 // do
#define WHILESYM 8 // while
#define VOIDSYM 9 // void
#define FORSYM 10 // for
#define SCANFSYM 11 // scanf
#define PRINTFSYM 12 // printf
#define RETURNSYM 13 // return
#define IDSYM 14 // identifier
#define PLUSSYM 15 // +
#define MINUSSYM 16 // -
#define MULTISYM 17 // *
#define DIVSYM 18 // /
#define BIGTHSYM 19 // >
#define SMALLTHSYM 20 // <
#define NOTBTHSYM 21 // <=
#define NOTSTHSYM 22 // >=
#define EQUSYM 23 // ==
#define NOTESYM 24 // !=
#define COMMASYM 25 // ,
#define LBPARENSYM 26 // {
#define RBPARENSYM 27 // }
#define LMPARENSYM 28 // [
#define RMPARENSYM 29 // ]
#define LPARENSYM 30 // (
#define RPARENSYM 31 // )
#define SQUOTESYM 32 // '
#define DQUOTESYM 33 // "
#define ASSIGNSYM 34 // =
#define STRINGSYM 35 // "it's a string"
#define COLONSYM 36 // :
#define SEMICSYM 37 // ;
#define DIGSYM 38 // number
#define LETTERSYM 43 //字符
//////////////////////warning///////////////////
#define WARNING "warning"
#define WARNING_0SURPLUS 39 // 012344
//////////////////////error/////////////////////
#define ERROR "error"
#define ERROR_OPERATION 40 // !
#define ERROR_FORMAT 41 // ' ' " "
#define ERROR_SYMBOL 42 