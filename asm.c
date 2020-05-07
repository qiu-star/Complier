#include "asm.h"

void initLex(char *fname);
void getSym();

FILE *mipsf;

//全局变量/常量表
GlobalTab globalTab;
//局部变量/常量表
LocalTab localTab;

//四元式表
FourVarCodeTab midcodeTab;
//目前扫描到的位置（四元式表）
int pos = 0;
int offset = 0;//$sp相对于$fp的偏移

//写文件的字符串常量
char tmp[128];

/**
 * 是不是数字
 * @return 是数字返回1,不是数字返回0
 */ 
int isNum(char *str)
{
    //因为变量名第一个字符肯定不是数字，所以根据这个来判断
    if(str[0] >= '0' && str[0] <= '9')
    {
        return 1;
    }
    return 0;
}

/**
 * 插入临时变量地址（地址为相对$sp的偏移量）
 */ 
void insertAddr(char *name)
{
    int len = localTab.len;
    localTab.l[len].name = name;
    localTab.l[len].addr = offset;
    localTab.len = len + 1;

    //$sp = $sp -4
    sprintf(tmp,"\t\tsubi\t$sp\t$sp\t4\n");
    fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
    offset += 4;
}

/**
 * 插入临时变量地址（数组）
 * @param name 数组名
 * @param space 数组大小
 */ 
void insertArryAddr(char *name, int space)
{
    int len = localTab.len;
    localTab.l[len].name = name;
    localTab.l[len].addr = offset;
    localTab.len = len + 1;

    //$sp = $sp - space*4
    //如a[1]，a[0]存sp，下一块地址为sp=sp-4
    sprintf(tmp,"\t\tsubi\t$sp\t$sp\t%d\n",4*space);
    fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
    offset += 4 * space;
}

/**
 * 在全局变量表中查找
 * @return 1为找到了，0为没找到
 */ 
int findInGlobalTab(char *name)
{
    for(int i = 0; i < globalTab.len; i++)
    {
        if(strcmp(globalTab.g[i].name, name) == 0)
        {
            return 1;
        }
    }
    return 0;
}

/**
 * 在局部变量表中查找
 * @return 如果找到了返回地址，如果没找到返回-1
 */ 
int findInLocalTab(char *name)
{
    for(int i = 0; i < localTab.len; i++)
    {
        if(strcmp(localTab.l[i].name, name) == 0)
        {
            return localTab.l[i].addr;
        }
    }
    return -1;
}

/**
 * 定义全局变量
 */ 
void asmGlobalData()
{
    sprintf(tmp,".data\n");
    fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
    //常量定义
    asmGlobalConstDefine();
    //变量定义
    asmGlobalVarDefine();
    //对print函数里的字符串进行处理
    asmPrintStringDefine();

    sprintf(tmp, ".text\n");
    fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
    sprintf(tmp, ".globl main\n");
    fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
    sprintf(tmp, "\t\tj\tmain\n");
    fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
}

void asmGlobalConstDefine()
{
    int i,len;
    for(i =0; strcmp("const",midcodeTab.element[i].op) == 0 && i < midcodeTab.length; i++)
    {
        //const四元式const, int, 12, a
        len = globalTab.len;
        globalTab.g[len].name = midcodeTab.element[i].rst;
        globalTab.g[len].value = midcodeTab.element[i].num_b;
        //g:	.word	5
        sprintf(tmp,"%s:\t.word\t%s\n",globalTab.g[len].name,globalTab.g[len].value);
        fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
        globalTab.len = ++len;
    }
    pos = i;
}

void asmGlobalVarDefine()
{
    int i = pos,len;
    while( (strcmp(midcodeTab.element[i].op, "int") == 0 || 
        strcmp(midcodeTab.element[i].op, "char") == 0 ||
        strcmp(midcodeTab.element[i].op, "inta") == 0 ||
        strcmp(midcodeTab.element[i].op, "chara") == 0) && i < midcodeTab.length)
    {
        len = globalTab.len;
        globalTab.g[len].name = midcodeTab.element[i].rst;
        globalTab.g[len].value = "";
        if(strcmp(midcodeTab.element[i].op, "inta") == 0 ||
          strcmp(midcodeTab.element[i].op, "chara") == 0)
        {
            //a:	.space	20
            sprintf(tmp,"%s:\t.space\t%d\n",globalTab.g[len].name,atoi(midcodeTab.element[i].num_b)*4);
        }
        else
        {
            sprintf(tmp,"%s:\t.space\t4\n",globalTab.g[len].name);
        }
        fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
        i++;
        globalTab.len = ++len;
    }
    pos = i;
}

/**
 * 将函数声明中的参数转换为mips
 * 默认函数的参数个数小于4,大于4的还要另加寄存器
 */ 
void asmParam()
{   
    int paramNum = 0;
    while(strcmp(midcodeTab.element[pos].op,"para") == 0)
    {
        switch (paramNum)
        {
        case 0:
            //参数存在a0-a4
            sprintf(tmp,"\t\tmove\t$t0\t$a0\n");
            fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
            //将参数压栈
            sprintf(tmp,"\t\tsw\t$t0\t-8($fp)\n");
            fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
            paramNum++;
            break;
        case 1:
            //参数存在a0-a4
            sprintf(tmp,"\t\tmove\t$t0\t$a1\n");
            fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
            //将参数压栈
            sprintf(tmp,"\t\tsw\t$t0\t-12($fp)\n");
            fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
            paramNum++;
            break;
        case 3:
            //参数存在a0-a4
            sprintf(tmp,"\t\tmove\t$t0\t$a2\n");
            fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
            //将参数压栈
            sprintf(tmp,"\t\tsw\t$t0\t-16($fp)\n");
            fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
            paramNum++;
            break;
        case 4:
            //参数存在a0-a4
            sprintf(tmp,"\t\tmove\t$t0\t$a3\n");
            fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
            //将参数压栈
            sprintf(tmp,"\t\tsw\t$t0\t-20($fp)\n");
            fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
            paramNum++;
            break;
        default:
            break;
        }
        insertAddr(midcodeTab.element[pos].rst);
        pos++;
    }
}

/**
 * 常量说明
 * @TODO
 */
void asmConstDeclare()
{
    //为这几个局部变量开辟栈空间
} 

/**
 * 变量说明
 */ 
void asmVarDeclare()
{
    //为这几个局部变量开辟栈空间
    while(strcmp(midcodeTab.element[pos].op,"inta") == 0 || 
    strcmp(midcodeTab.element[pos].op,"chara") == 0 ||
    strcmp(midcodeTab.element[pos].op,"char") == 0 ||
    strcmp(midcodeTab.element[pos].op,"int") == 0)
    {
        if(strcmp(midcodeTab.element[pos].op,"inta") == 0 || 
        strcmp(midcodeTab.element[pos].op,"chara") == 0)
        {
            insertArryAddr(midcodeTab.element[pos].rst,atoi(midcodeTab.element[pos].num_b));
        }
        else
        {
            insertAddr(midcodeTab.element[pos].rst);
        }
        
        pos++;
    }
}

/**
 * >
 */ 
void asmBt()
{
    if(isNum(midcodeTab.element[pos].num_a))
    {
        //li $t0 1
        sprintf(tmp,"\t\tli\t$t0\t%s\n",midcodeTab.element[pos].num_a);
        fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
    }
    else
    {
        //如果num_a不是数字
        int addr = findInLocalTab(midcodeTab.element[pos].num_a);
        //是否为局部变量
        if(addr != -1)
        {
            //addr是相对$fp来说的
            sprintf(tmp,"\t\tlw\t$t0\t%d($fp)\n",-1*addr);
            fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
        }
        //否则是否为全局变量（优先使用局部变量）
        else if(findInGlobalTab(midcodeTab.element[pos].num_a))
        {
            //la $t0 GLOBAL
            //lw $t0 ($t0)
            sprintf(tmp,"\t\tla\t$t0\t%s\n",midcodeTab.element[pos].num_a);
            fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
            sprintf(tmp,"\t\tlw\t$t0\t($t0)\n");
            fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
        }
        else
        {
            fprintf(stderr,"> : '%s\n' don't exist!\n",midcodeTab.element[pos].num_a);
            exit(1);
        }
    }
    
    if(isNum(midcodeTab.element[pos].num_b))
    {
        //li $t1 1
        sprintf(tmp,"\t\tli\t$t1\t%s\n",midcodeTab.element[pos].num_b);
        fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
    }
    else
    {
        //如果num_b不是数字
        int addr = findInLocalTab(midcodeTab.element[pos].num_b);
        //是否为局部变量
        if(addr != -1)
        {
            //addr是相对$fp来说的
            sprintf(tmp,"\t\tlw\t$t1\t%d($fp)\n",-1*addr);
            fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
        }
        //否则是否为全局变量（优先使用局部变量）
        else if(findInGlobalTab(midcodeTab.element[pos].num_b))
        {
            //la $t0 GLOBAL
            //lw $t0 ($t0)
            sprintf(tmp,"\t\tla\t$t1\t%s\n",midcodeTab.element[pos].num_b);
            fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
            sprintf(tmp,"\t\tlw\t$t1\t($t1)\n");
            fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
        }
        else
        {
            fprintf(stderr,"> : '%s\n' don't exist!\n",midcodeTab.element[pos].num_a);
            exit(1);
        }
    }
    
    //ble r, s, tag 	r <= s -> tag
    //也就是说不符合r > s这个条件就会跳转到tag
    sprintf(tmp,"\t\tble\t$t0\t$t1\t");//tag等到jne这个命令的时候写
    fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
    pos++;
}

/**
 * <
 */
void asmSt()
{

}

/**
 * jne, , , _LABEL_0
 */ 
void asmJumpToLable()
{
    sprintf(tmp,"%s\n",midcodeTab.element[pos].rst);
    fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
    pos++;
}

/**
 * 语句列
 */ 
void asmStates()
{
    //函数没结束
    while(strcmp(midcodeTab.element[pos].op,"end") != 0)
    {
        /*if语句判断条件*/
        if(strcmp(midcodeTab.element[pos].op,">") == 0)
        {
            asmBt();
        }
        else if(strcmp(midcodeTab.element[pos].op,">=") == 0)
        {
            
        }
        else if(strcmp(midcodeTab.element[pos].op,"<") == 0)
        {
            asmStates();
        }
        else if(strcmp(midcodeTab.element[pos].op,"<=") == 0)
        {
            
        }
        else if(strcmp(midcodeTab.element[pos].op,"==") == 0)
        {
            
        }
        else if(strcmp(midcodeTab.element[pos].op,"!=") == 0)
        {
            
        }
        else if(strcmp(midcodeTab.element[pos].op,"jne") == 0)
        {
            asmJumpToLable();
            break;
        }
    }
}

/**
 * 复合语句
 */ 
void asmCompoundState()
{
    /*全局变量声明*/
    asmConstDeclare();
    /*局部变量声明*/
    asmVarDeclare();
    /*语句列*/
    asmStates();
}

/**
 * 将函数转换为mips
 */ 
void asmFunc()
{
    char tmp[512];
    //函数名
    sprintf(tmp,"%s:\n",midcodeTab.element[pos].rst);
    fwrite(tmp,sizeof(char),strlen(tmp),mipsf);

    /*保存现场*/
    //将原$fp压栈
    sprintf(tmp,"\t\tsw\t$fp\t($sp)\n");
    fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
    //设置本函数$fp = $sp
    sprintf(tmp,"\t\tadd\t$fp\t$sp\t$0\n");
    fwrite(tmp,sizeof(char),strlen(tmp),mipsf);

    //$sp = $sp -4
    sprintf(tmp,"\t\tsubi\t$sp\t$sp\t4\n");
    fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
    offset = 4;
    //将$ra压栈，返回地址存在$ra中
    sprintf(tmp,"\t\tsw\t$ra\t($sp)\n");
    fwrite(tmp,sizeof(char),strlen(tmp),mipsf);

    //$sp = $sp -4,更新栈帧位置
    sprintf(tmp,"\t\tsubi\t$sp\t$sp\t4\n");
    fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
    offset += 4;
    pos++;
    /*调用参数，默认参数个数小于等于4   */
    asmParam();
    
    /*复合语句*/
    asmCompoundState();

    /*恢复现场*/

    printLocalVarTable();
}

void asmPrintStringDefine()
{
    int i, cnt = 0;
    char tmp[512];
    for(i = pos; i < midcodeTab.length; i++)
    {
        if(strcmp(midcodeTab.element[i].op, "prtf") == 0)
        {
            if(strcmp(midcodeTab.element[i].num_a, "") != 0)
            {
                int len = globalTab.len;
                char name[20];
                sprintf(name,"$string%d",cnt);
                globalTab.g[len].name = String(name);
                globalTab.g[len].value = midcodeTab.element[i].num_a;
                sprintf(tmp, "%s:\t.asciiz\t\"%s\"\n", name, midcodeTab.element[i].num_a);
                fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
                //定义好常量后，把四元式从prtf, false, $_13, int换成prtf,$string1,$_13,int
                midcodeTab.element[i].num_a = globalTab.g[len].name;
                globalTab.len = ++len;
                cnt++;
            }
        }
    }
}

void printGlobalTable()
{
    printf("------------------global table------------------\n");
    for(int i = 0; i < globalTab.len; i++)
    {
        printf("name: %s\t\tvalue: %s\n",globalTab.g[i].name,globalTab.g[i].value);
    }
}

void printLocalVarTable()
{
    printf("------------------var table------------------\n");
    for(int i =0; i < localTab.len; i++)
    {
        printf("name: %s\t\taddr: %d\n",localTab.l[i].name,localTab.l[i].addr);
    }
}

void asmRun()
{
    midcodeTab = getMidCode();
    //程序设定：先定义全局变量、然后是程序
    mipsf = fopen("result/mips.txt","w");
    asmGlobalData();
    asmFunc();
    // while(pos < midcodeTab.length)
    // {
    //     if(strcmp(midcodeTab.element[pos].op,"func") == 0)//如果是函数func, , , main
    //     {

    //     }
    // }

    printGlobalTable();
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr,"usage: a.out filename\n"); 
        exit(1);
    }

    //初始化
    initFourVarCodeTab();
    initSymTab();
    initLex(argv[1]);

    //语法分析
    getSym();
    programme();

    //语法分析结果输出
    printSymTab();
    printFourVarCodeTab();

    asmRun();
    return 0;
}