#include "grammar.h"

//全局变量/常量的数据结构
typedef struct Global
{
    string name;
    string value;
}Global;
//全局变量/常量表
typedef struct GlobalTab
{
    Global g[512];
    int len;
}GlobalTab;

//局部变量/常量
typedef struct Local
{
    string name;
    int addr;
}Local;
//局部变量/常量表
typedef struct LocalTab
{
    Local l[512];
    int len;
}LocalTab;

void insertAddr(char *name);
void insertArryAddr(char *name, int space);
int findInGlobalTab(char *name);
int isNum(char *str);
void asmLoad(string var, string reg);
void asmStore(string reg, string var);

void asmBt();
void asmSt();
void asmNbt();
void asmNst();
void asmEq();
void asmNeq();
void asmJumpToLable();
void asmGlobalData();
void asmGlobalConstDefine();
void asmGlobalVarDefine();
void asmPrintStringDefine();
void asmFunc();
void asmParam();
void asmAdd();
void asmSub();
void asmMul();
void asmDiv();
void asmAssign();
void asmJmp();
void asmLabel();
void asmPrintf();
void asmCall();
void asmEnd();
void asmCallParam();

void printLocalVarTable();
void printGlobalTable();