%{
#include "util.h"
#include "tokens.h"

int yywrap(void)
{
 return 1;
}

void removeDoubleQuotation(char * text)
{
    //将开头和结尾的双引号去掉
    int len = strlen(text);
    int i = 0;
    for(i = 1; i < len-1; i++)
    {
        text[i-1] = text[i];
    }
    text[len - 2] = '\0';
}

%}

%%
" "|"\t"                {continue;}
"\n"	                {continue;}
"int"                   {return INTSYM;}
"char"                  {return CHARSYM;}
"const"                 {return CONSTSYM;}
"main"                  {return MAINSYM;}
"if"                    {return IFSYM;}
"else"                  {return ELSESYM;}
"do"                    {return DOSYM;}
"while"                 {return WHILESYM;}
"void"                  {return VOIDSYM;}
"for"                   {return FORSYM;}
"scanf"                 {return SCANFSYM;}
"printf"                {return PRINTFSYM;}
"return"                {return RETURNSYM;}
[_a-zA-Z][_a-zA-Z0-9]*  {yylval.sval=String(yytext); return IDSYM;}
"+"                     {return PLUSSYM;}
"-"                     {return MINUSSYM;}
"*"                     {return MULTISYM;}
"/"                     {return DIVSYM;}
">"                     {return BIGTHSYM;}
"<"                     {return SMALLTHSYM;}
"<="                    {return NOTBTHSYM;}
">="                    {return NOTSTHSYM;}
"=="                    {return EQUSYM;}
"!="                    {return NOTESYM;}
","                     {return COMMASYM;}
"{"                     {return LBPARENSYM;}
"}"                     {return RBPARENSYM;}
"["                     {return LMPARENSYM;}
"]"                     {return RMPARENSYM;}
"("                     {return LPARENSYM;}
")"                     {return RPARENSYM;}
"'"                     {return SQUOTESYM;}
"\""                    {return DQUOTESYM;}
"="                     {return ASSIGNSYM;}
"\""[^"]*"\""   	    {removeDoubleQuotation(yytext); yylval.sval=String(yytext); return STRINGSYM;}
"'"[^']*"'"             {char *t= (char*)malloc(2*sizeof(char));t[0]=yytext[1];t[1]='\0';yylval.sval=String(t); return LETTERSYM;}
":"                     {return COLONSYM;}
";"                     {return SEMICSYM;}
[0-9]+	                {yylval.ival=atoi(yytext); return DIGSYM;}
"/*".*"*/"  		    {}
.                       {return ERROR_SYMBOL;}
%%
