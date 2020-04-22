#include "util.h"
#include "tokens.h"
#include <stdio.h>
#include <stdlib.h>
#include "symbol.h"
#include "midcode.h"
#include <string.h>

void constDefine(int type);
void constDeclare();
void varDefine(int type);
void varDeclare(int type);
string factor();
string item();
string expression();
int valuePara();
void state();
void ifState();
void condition();
void loopState();
void scanfState();
void printState();
void returnState();