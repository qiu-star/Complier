#include <stdio.h>
#include <stdlib.h>
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


string tokname(int tok) 
{
    return toknames[tok];
}

//有头指针的链表
token* lexical_analysis(char *fname)
{
    printf("--------------------词法分析--------------------\n");
    int tok;
    token *head = checked_malloc(sizeof(*head));
    token *ph = head;
    EM_reset(fname);
    for(;;)
    {
        tok = yylex();
        if(tok == 0) break;

        token *tmp = checked_malloc(sizeof(*tmp)); 

        switch(tok) 
        {
            case ID: case STRING: case BOUND:
                tmp->type = tok;
                tmp->value.sval = yylval.sval;
                printf("%10s %4d %s\n",tokname(tok),EM_tokPos,yylval.sval);
                break;
            case CHAR:
                tmp->type = tok;
                tmp->value.cval = yylval.cval;
                printf("%10s %4d %c\n", tokname(tok),EM_tokPos,yylval.cval);
                break;
            case INT:
                tmp->type = tok;
                tmp->value.ival = yylval.ival;
                printf("%10s %4d %d\n",tokname(tok),EM_tokPos,yylval.ival);
                break;
            case FLOAT:
                tmp->type = tok;
                tmp->value.fval = yylval.fval;
                printf("%10s %4d %f\n", tokname(tok), EM_tokPos,yylval.fval);
                break;
            default:
                tmp->type = tok;
                printf("%10s %4d\n",tokname(tok),EM_tokPos);
                break;
        }
        ph->next = tmp;
        ph = ph->next;
    }
    return head;
}


// void print(token* head)
// {
//     printf("-------------------------------------------------------------\n");
//     int tok;
//     token* tmp = head->next;
//     while(tmp)
//     {
//         tok = tmp->type;
//         switch (tok)
//         {
//             case ID: case STRING: case BOUND:
//                 printf("%10s %s\n",tokname(tok),tmp->value.sval);
//                 break;
//             case CHAR:
//                 printf("%10s %c\n", tokname(tok),tmp->value.cval);
//                 break;
//             case INT:
//                 printf("%10s %d\n",tokname(tok),tmp->value.ival);
//                 break;
//             case FLOAT:
//                 printf("%10s %f\n", tokname(tok),tmp->value.fval);
//                 break;
//             default:
//                 printf("%10s\n",tokname(tok));
//                 break;
//         }
//         tmp = tmp -> next;
//     }
// }

int main(int argc, char **argv) 
{
    
    if (argc != 2)
    {
        fprintf(stderr,"usage: a.out filename\n"); 
        exit(1);
    }

    token* tokenlist_head = lexical_analysis(argv[1]);
    // print(tokenlist_head);
    return 0;
}


