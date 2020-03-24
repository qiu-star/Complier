#include <stdio.h>
#include "util.h"
#include "errormsg.h"
#include "tokens.h"

YYSTYPE yylval;

int yylex(void); /* prototype for the lexing function */



string toknames[] = {
"ERROR","STRING","INT","CHAR","FLOAT","BOUND","ID","KW_INT","KW_CHAR","KW_AUTO",
"KW_FLOAT","KW_STRUCT","KW_UNION","KW_ENUM","KW_TYPEDEF","KW_CONST","KW_EXTERN",
"KW_REGISTER","KW_STATIC","KW_VOLATILE","KW_VOID","KW_ELSE","KW_SWITCH","KW_CASE",
"KW_FOR","KW_DO","KW_WHILE","KW_GOTO","KW_CONTINUE","KW_BREAK","KW_DEFAULT","KW_SIZEOF",
"KW_RETURN","KW_IF","KW_PRINTF","KW_SCANF","END"
};


string tokname(tok) {
  return toknames[tok];
}

int main(int argc, char **argv) {
 string fname; int tok;
 if (argc!=2) {fprintf(stderr,"usage: a.out filename\n"); exit(1);}
 fname=argv[1];
 EM_reset(fname);
 for(;;) {
   tok=yylex();
   if (tok==0) break;
   switch(tok) {
   case ID: case STRING: case BOUND:
     printf("%10s %4d %s\n",tokname(tok),EM_tokPos,yylval.sval);
     break;
   case CHAR:
     printf("%10s %4d %c\n", tokname(tok),EM_tokPos,yylval.cval);
     break;
   case INT:
     printf("%10s %4d %d\n",tokname(tok),EM_tokPos,yylval.ival);
     break;
   case FLOAT:
     printf("%10s %4d %f\n", tokname(tok), EM_tokPos,yylval.fval);
     break;
   default:
     printf("%10s %4d\n",tokname(tok),EM_tokPos);
   }
 }
 return 0;
}


