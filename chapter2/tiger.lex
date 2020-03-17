%{
#include <string.h>
#include "util.h"
#include "tokens.h"
#include "errormsg.h"

int charPos=1;

int yywrap(void)
{
 charPos=1;
 return 1;
}


void adjust(void)
{
 EM_tokPos=charPos;
 charPos+=yyleng;
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
" "|"\t"                {adjust(); continue;}
"\n"	                {adjust(); EM_newline(); continue;}
","	                    {adjust(); return COMMA;}
"for"  	                {adjust(); return FOR;}
":"                     {adjust(); return COLON;}
";"                     {adjust(); return SEMICOLON;}
"("                     {adjust(); return LPAREN;}
")"                     {adjust(); return RPAREN;}
"["                     {adjust(); return LBRACK;}
"]"                     {adjust(); return RBRACK;}
"{"                     {adjust(); return LBRACE;}
"}"                     {adjust(); return RBRACE;}
"."                     {adjust(); return DOT;}
"+"                     {adjust(); return PLUS;}
"-"                     {adjust(); return MINUS;}
"*"                     {adjust(); return TIMES;}
"/"                     {adjust(); return DIVIDE;}
"="                    {adjust(); return EQ;}
"!="                    {adjust(); return NEQ;}
"<"                     {adjust(); return LT;}
"<="                    {adjust(); return LE;}
">"                     {adjust(); return GT;}
">="                    {adjust(); return GE;}
"&"                    {adjust(); return AND;}
"|"                    {adjust(); return OR;}
":="                    {adjust(); return ASSIGN;}
"array"                 {adjust(); return ARRAY;}
"if"                    {adjust(); return IF;}
"then"                  {adjust(); return THEN;}
"else"                  {adjust(); return ELSE;}
"while"                 {adjust(); return WHILE;}
"to"                    {adjust(); return TO;}
"do"                    {adjust(); return DO;}
"let"                   {adjust(); return LET;}
"in"                    {adjust(); return IN;}
"end"                   {adjust(); return END;}
"of"                    {adjust(); return OF;}
"break"                 {adjust(); return BREAK;}
"nil"                   {adjust(); return NIL;}
"function"              {adjust(); return FUNCTION;}
"var"                   {adjust(); return VAR;}
"type"                  {adjust(); return TYPE;}
"print"                 {adjust(); return PRINT;}
[0-9]+	                {adjust(); yylval.ival=atoi(yytext); return INT;}
[_a-zA-Z][_a-zA-Z0-9]*  {adjust(); yylval.sval=String(yytext); return ID;}
"/*"[ a-zA-Z0-9_/\-;.:=()\[\]"'\\]*"*/"   {adjust();}
"\""[ a-zA-Z0-9_/\-;.:=()\[\]\\]*"\""   {adjust(); removeDoubleQuotation(yytext); yylval.sval=String(yytext); return STRING;}
[0-9]+"."[0-9]+         {adjust(); yylval.fval=atof(yytext); return REAL;}
.	                    {adjust(); EM_error(EM_tokPos,"illegal token");}
%%
