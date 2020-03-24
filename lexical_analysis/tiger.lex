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
"\n"	                {adjust(); continue;}
"+"|"-"|"*"|"/"|"&&"|"||"|"&"|"|"|">="|"<="|">"|"<"|"=="|"!="|"("|")"|"{"|"}"|"["|"]"|";"|","|"="   {adjust(); yylval.sval=String(yytext); return BOUND;}
"int"                   {adjust(); return KW_INT;}
"char"                  {adjust(); return KW_CHAR;}
"auto"                  {adjust(); return KW_AUTO;}
"float"                 {adjust(); return KW_FLOAT;}
"struct"                {adjust(); return KW_STRUCT;}
"union"                 {adjust(); return KW_UNION;}
"enum"                  {adjust(); return KW_ENUM;}
"typedef"               {adjust(); return KW_TYPEDEF;}
"const"                 {adjust(); return KW_CONST;}
"extern"                {adjust(); return KW_EXTERN;}
"register"              {adjust(); return KW_REGISTER;}
"static"                {adjust(); return KW_STATIC;}
"volatile"              {adjust(); return KW_VOLATILE;}
"void"                  {adjust(); return KW_VOID;}
"else"                  {adjust(); return KW_ELSE;}
"switch"                {adjust(); return KW_SWITCH;}
"case"                  {adjust(); return KW_CASE;}
"for"                   {adjust(); return KW_FOR;}
"do"                    {adjust(); return KW_DO;}
"while"                 {adjust(); return KW_WHILE;}
"goto"                  {adjust(); return KW_GOTO;}
"continue"              {adjust(); return KW_CONTINUE;}
"break"                 {adjust(); return KW_BREAK;}
"default"               {adjust(); return KW_DEFAULT;}
"sizeof"                {adjust(); return KW_SIZEOF;}
"return"                {adjust(); return KW_RETURN;}
"if"                    {adjust(); return KW_IF;}
"printf"                {adjust(); return KW_PRINTF;}
"scanf"                 {adjust(); return KW_SCANF;}
"'"[^']"'"              {adjust(); yylval.cval=yytext[1]; return CHAR;}
[0-9]+"."[0-9]+         {adjust(); yylval.fval=atof(yytext); return FLOAT;}
[0-9]+	                {adjust(); yylval.ival=atoi(yytext); return INT;}
[_a-zA-Z][_a-zA-Z0-9]*  {adjust(); yylval.sval=String(yytext); return ID;}
"/*".*"*/"  		    {adjust();}
"\""[^"]*"\""   	    {adjust(); removeDoubleQuotation(yytext); yylval.sval=String(yytext); return STRING;}
.                       {adjust(); return ERROR;}
%%
