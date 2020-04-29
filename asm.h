#include "grammar.h"

void asmGlobalData();
void asmGlobalConstDefine();
void asmGlobalVarDefine();
void asmPrintStringDefine();

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

//过程记录
