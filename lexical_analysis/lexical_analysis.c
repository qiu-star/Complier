#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "tokens.h"

int symID;

extern FILE *yyin;
YYSTYPE yylval;

int yylex(void); /* prototype for the lexing function */

string toknames[] = {
"","INTSYM","CHARSYM","CONSTSYM","MAINSYM","IFSYM","ELSESYM","DOSYM","WHILESYM","VOIDSYM","FORSYM",
"SCANFSYM","PRINTFSYM","RETURNSYM","IDSYM","PLUSSYM","MINUSSYM","MULTISYM",
"DIVSYM","BIGTHSYM","SMALLTHSYM","NOTBTHSYM","NOTSTHSYM","EQUSYM","NOTESYM",
"COMMASYM","LBPARENSYM","RBPARENSYM","LMPARENSYM","RMPARENSYM","LPARENSYM","RPARENSYM","SQUOTESYM",
"DQUOTESYM","ASSIGNSYM","STRINGSYM","COLONSYM","SEMICSYM","DIGSYM","WARNING_0SURPLUS",
"ERROR_OPERATION","ERROR_FORMAT","ERROR_SYMBOL","LETTERSYM"
};


string tokname(int tok) 
{
    return toknames[tok];
}

void initLex(char *fname)
{
    yyin = fopen(fname,"r");
}


void getSym()
{
    symID = yylex();
}


// //分析结果写在文件中
// void lexical_analysis_test(char *fname)
// {
//     printf("--------------------词法分析--------------------\n");
//     int tok;
//     yyin = fopen(fname,"r");
//     for(;;)
//     {
//         tok = yylex();
//         if(tok == 0) break;

//         switch(tok) 
//         {
//             case IDSYM: case STRINGSYM:
//                 printf("%10s %s\n",tokname(tok),yylval.sval);
//                 break;
//             case DIGSYM:
//                 printf("%10s %d\n",tokname(tok),yylval.ival);
//                 break;
//             default:
//                 printf("%10s\n",tokname(tok));
//                 break;
//         }
//     }
// }

// int main(int argc, char **argv) 
// {
    
//     if (argc != 2)
//     {
//         fprintf(stderr,"usage: a.out filename\n"); 
//         exit(1);
//     }

//     lexical_analysis_test(argv[1]);
//     return 0;
// }


