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
        default://参数个数大于4时，会在调用函数时把参数压栈，也就是说直接为参数开辟这个栈空间就行
            break;
        }
        insertAddr(midcodeTab.element[pos].rst);
        pos++;
    }
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
 * 加载变量到寄存器
 * @param var 变量名
 * @param reg 寄存器名
 */ 
void asmLoad(string var, string reg)
{
    if(var[0] == '-' || isNum(var))
    {
        //li $t0 1
        sprintf(tmp,"\t\tli\t%s\t%s\n",reg,var);
        fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
    }
    else
    {
         //如果var不是数字
        int addr = findInLocalTab(var);
        //是否为局部变量
        if(addr != -1)
        {
            //addr是相对$fp来说的
            sprintf(tmp,"\t\tlw\t%s\t%d($fp)\n",reg,-1*addr);
            fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
        }
        //否则是否为全局变量（优先使用局部变量）
        else if(findInGlobalTab(var))
        {
            //la $t0 GLOBAL
            //lw $t0 ($t0)
            sprintf(tmp,"\t\tla\t%s\t%s\n",reg,var);
            fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
            sprintf(tmp,"\t\tlw\t%s\t(%s)\n",reg,reg);
            fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
        }
        else
        {
            fprintf(stderr,"mips load: '%s' don't exist!\n",var);
            exit(1);
        }
    }
    
}

/**
 * 将寄存器的值存储到变量
 * @param reg 寄存器名
 * @param var 变量名
 */ 
void asmStore(string reg, string var)
{
    int addr;
    //首先这个变量是否为中间变量
    if(var[0] == '$')
    {
        //在栈中为这个中间变量开辟空间
        insertAddr(var);
        //得到中间变量地址
        addr = findInLocalTab(var);
        //sw $t0 offset($fp)
        sprintf(tmp,"\t\tsw\t%s\t%d($fp)\n",reg,-1*addr);
        fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
    }
    else
    {
        //先看这个变量是否为局部
        addr = findInLocalTab(var);
        if(addr != -1)
        {
            //sw $t0 offset($fp)
            sprintf(tmp,"\t\tsw\t%s\t%d($fp)\n",reg,-1*addr);
            fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
        }
        else if(findInGlobalTab(var))//否则再看这个变量是否为全局
        {
            //la $t1 LABEL
            sprintf(tmp,"\t\tla\t$t1\t%s\n",var);
            fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
            //sw $t0 ($t1)
            sprintf(tmp,"\t\tsw\t%s\t($t1)\n",reg);
            fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
        }
        else
        {
            fprintf(stderr,"mips store: '%s\n' don't exist!\n",var);
            exit(1);
        }
    }
    
}

/**
 * >
 */ 
void asmBt()
{
    asmLoad(midcodeTab.element[pos].num_a,"$t0");
    asmLoad(midcodeTab.element[pos].num_b,"$t1");
    
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
    asmLoad(midcodeTab.element[pos].num_a,"$t0");
    asmLoad(midcodeTab.element[pos].num_b,"$t1");
    
    //bge r, s, tag 	r >= s -> tag
    //也就是说不符合r > s这个条件就会跳转到tag
    sprintf(tmp,"\t\tbge\t$t0\t$t1\t");//tag等到jne这个命令的时候写
    fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
    pos++;
}

/**
 * <=
 */ 
void asmNbt()
{
    asmLoad(midcodeTab.element[pos].num_a,"$t0");
    asmLoad(midcodeTab.element[pos].num_b,"$t1");
    
    //bgt r, s, tag 	r > s -> tag
    //也就是说不符合r > s这个条件就会跳转到tag
    sprintf(tmp,"\t\tbgt\t$t0\t$t1\t");//tag等到jne这个命令的时候写
    fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
    pos++;
}

/**
 * >=
 */ 
void asmNst()
{
    asmLoad(midcodeTab.element[pos].num_a,"$t0");
    asmLoad(midcodeTab.element[pos].num_b,"$t1");
    
    //blt r, s, tag 	r < s -> tag
    //也就是说不符合r < s这个条件就会跳转到tag
    sprintf(tmp,"\t\tblt\t$t0\t$t1\t");//tag等到jne这个命令的时候写
    fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
    pos++;
}

/**
 * ==
 */ 
void asmEq()
{
    asmLoad(midcodeTab.element[pos].num_a,"$t0");
    asmLoad(midcodeTab.element[pos].num_b,"$t1");
    
    //bne r, s, tag 	r != s -> tag
    //也就是说不符合r != s这个条件就会跳转到tag
    sprintf(tmp,"\t\tbne\t$t0\t$t1\t");//tag等到jne这个命令的时候写
    fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
    pos++;
}

void asmNeq()
{
    asmLoad(midcodeTab.element[pos].num_a,"$t0");
    asmLoad(midcodeTab.element[pos].num_b,"$t1");
    
    //beq r, s, tag 	r = s -> tag
    //也就是说不符合r == s这个条件就会跳转到tag
    sprintf(tmp,"\t\tbeq\t$t0\t$t1\t");//tag等到jne这个命令的时候写
    fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
    pos++;
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
 * 调用函数
 */ 
void asmCall()
{
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

    //将$ra压栈，返回地址存在$ra中
    sprintf(tmp,"\t\tsw\t$ra\t($sp)\n");
    fwrite(tmp,sizeof(char),strlen(tmp),mipsf);

    //call, cmp, ,  =>  jal cmp
    sprintf(tmp,"\t\tjal\t%s\n",midcodeTab.element[pos].num_a);
    fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
    sprintf(tmp,"\t\tnop\n");
    fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
    //f = cmp(1,2);对应的四元式：
    //call, cmp, , $_5
    //=, $_5, , f
    if(strcmp(midcodeTab.element[pos].rst, "") != 0)
    {
        insertAddr(midcodeTab.element[pos].rst);
        int addr = findInLocalTab(midcodeTab.element[pos].rst);
        //返回值存在$v1
        sprintf(tmp,"\t\tsw\t$v1\t%d($fp)\n",-1*addr);
        fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
    }
    pos++;
}

/**
 * 调用函数的参数
 */ 
void asmCallParam()
{
    int calParamNum = 0;
    int addr;
    while(strcmp(midcodeTab.element[pos].op,"cpara") == 0)
    {
        addr = findInLocalTab(midcodeTab.element[pos].rst);
        //前四个参数存在$a0-$a3
        switch (calParamNum)
        {
        case 0:
            if(midcodeTab.element[pos].rst[0] == '-' || isNum(midcodeTab.element[pos].rst))
            {
                sprintf(tmp,"\t\tli\t$a0\t%s\n",midcodeTab.element[pos].rst);
                fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
            }
            else
            {
                if(addr != -1)//局部变量
                {
                    sprintf(tmp,"\t\tlw\t$a0\t%d($fp)\n",-1*addr);
                    fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
                }
                else if(findInGlobalTab(midcodeTab.element[pos].rst))
                {
                    sprintf(tmp,"\t\tla\t$t0\t%s\n",midcodeTab.element[pos].rst);
                    fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
                    sprintf(tmp,"\t\tlw\t$a0\t($t0)\n");
                    fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
                }
            }
            break;
        case 1:
            if(midcodeTab.element[pos].rst[0] == '-' || isNum(midcodeTab.element[pos].rst))
            {
                sprintf(tmp,"\t\tli\t$a1\t%s\n",midcodeTab.element[pos].rst);
                fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
            }
            else
            {
                if(addr != -1)//局部变量
                {
                    sprintf(tmp,"\t\tlw\t$a1\t%d($fp)\n",-1*addr);
                    fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
                }
                else if(findInGlobalTab(midcodeTab.element[pos].rst))
                {
                    sprintf(tmp,"\t\tla\t$t0\t%s\n",midcodeTab.element[pos].rst);
                    fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
                    sprintf(tmp,"\t\tlw\t$a1\t($t0)\n");
                    fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
                }
            }
            break;
        case 2:
            if(midcodeTab.element[pos].rst[0] == '-' || isNum(midcodeTab.element[pos].rst))
            {
                sprintf(tmp,"\t\tli\t$a2\t%s\n",midcodeTab.element[pos].rst);
                fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
            }
            else
            {
                if(addr != -1)//局部变量
                {
                    sprintf(tmp,"\t\tlw\t$a2\t%d($fp)\n",-1*addr);
                    fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
                }
                else if(findInGlobalTab(midcodeTab.element[pos].rst))
                {
                    sprintf(tmp,"\t\tla\t$t0\t%s\n",midcodeTab.element[pos].rst);
                    fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
                    sprintf(tmp,"\t\tlw\t$a2\t($t0)\n");
                    fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
                }
            }
            break;
        case 3:
            if(midcodeTab.element[pos].rst[0] == '-' || isNum(midcodeTab.element[pos].rst))
            {
                sprintf(tmp,"\t\tli\t$a3\t%s\n",midcodeTab.element[pos].rst);
                fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
            }
            else
            {
                if(addr != -1)//局部变量
                {
                    sprintf(tmp,"\t\tlw\t$a3\t%d($fp)\n",-1*addr);
                    fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
                }
                else if(findInGlobalTab(midcodeTab.element[pos].rst))
                {
                    sprintf(tmp,"\t\tla\t$t0\t%s\n",midcodeTab.element[pos].rst);
                    fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
                    sprintf(tmp,"\t\tlw\t$a3\t($t0)\n");
                    fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
                }
            }
            break;
        default:
            //存到栈中
            //调用函数的栈(以当前$sp为基准)：
            //$sp存$fp $sp-4存$ra $sp-8会把$a0存进去 $sp-12存$a1 $sp-16存$a2 $sp-20存$a3 也就是第四个参数后的参数依次存在$sp-24后
            if(midcodeTab.element[pos].rst[0] == '-' || isNum(midcodeTab.element[pos].rst))
            {
                sprintf(tmp,"\t\tli\t$t8\t%s\n",midcodeTab.element[pos].rst);
                fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
            }
            else
            {
                if(addr != -1)//局部变量
                {
                    sprintf(tmp,"\t\tlw\t$t8\t%d($fp)\n",-1*addr);
                    fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
                }
                else if(findInGlobalTab(midcodeTab.element[pos].rst))
                {
                    sprintf(tmp,"\t\tla\t$t0\t%s\n",midcodeTab.element[pos].rst);
                    fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
                    sprintf(tmp,"\t\tlw\t$t8\t($t0)\n");
                    fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
                }
            }
            //调用函数的栈(以当前$sp为基准)：
            //$sp存$fp $sp-4存$ra $sp-8会把$a0存进去 $sp-12存$a1 $sp-16存$a2 $sp-20存$a3 也就是第四个参数后的参数依次存在$sp-24后
            sprintf(tmp,"\t\tsw\t$t8\t%d($sp)\n",-1*((calParamNum-4)*4+24));
            fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
            break;
        }
        
        calParamNum++;
        pos++;
    }
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
    offset = 8;
    if(strcmp(midcodeTab.element[pos].rst,"main") == 0)
    {
        sprintf(tmp,"\t\tadd\t$fp\t$sp\t$0\n");
        fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
        offset = 4;
    }
    //$sp = $sp -4,更新栈帧位置
    sprintf(tmp,"\t\tsubi\t$sp\t$sp\t4\n");
    fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
    
    pos++;
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

/**
 * +
 */ 
void asmAdd()
{
    asmLoad(midcodeTab.element[pos].num_a,"$t0");
    asmLoad(midcodeTab.element[pos].num_b,"$t1");

    //add $t0 $t0 $t1
    sprintf(tmp,"\t\tadd\t$t0\t$t0\t$t1\n");
    fwrite(tmp,sizeof(char),strlen(tmp),mipsf);

    //sw $t0 offset($fp) or 全局变量情况
    asmStore("$t0",midcodeTab.element[pos].rst);
    pos++;
}

/**
 * -
 */ 
void asmSub()
{
    asmLoad(midcodeTab.element[pos].num_a,"$t0");
    asmLoad(midcodeTab.element[pos].num_b,"$t1");

    //sub $t0 $t0 $t1
    sprintf(tmp,"\t\tsub\t$t0\t$t0\t$t1\n");
    fwrite(tmp,sizeof(char),strlen(tmp),mipsf);

    //sw $t0 offset($fp) or 全局变量情况
    asmStore("$t0",midcodeTab.element[pos].rst);
    pos++;
}

/**
 * *
 */ 
void asmMul()
{
    asmLoad(midcodeTab.element[pos].num_a,"$t0");
    asmLoad(midcodeTab.element[pos].num_b,"$t1");

    //mul $t0 $t0 $t1
    sprintf(tmp,"\t\tmul\t$t0\t$t0\t$t1\n");
    fwrite(tmp,sizeof(char),strlen(tmp),mipsf);

    //sw $t0 offset($fp) or 全局变量情况
    asmStore("$t0",midcodeTab.element[pos].rst);
    pos++;
}

/**
 * /
 */ 
void asmDiv()
{
    asmLoad(midcodeTab.element[pos].num_a,"$t0");
    asmLoad(midcodeTab.element[pos].num_b,"$t1");

    //div $t0 $t0 $t1
    sprintf(tmp,"\t\tdiv\t$t0\t$t0\t$t1\n");
    fwrite(tmp,sizeof(char),strlen(tmp),mipsf);

    //sw $t0 offset($fp) or 全局变量情况
    asmStore("$t0",midcodeTab.element[pos].rst);
    pos++;
}

/**
 * =
 */ 
void asmAssign()
{
    asmLoad(midcodeTab.element[pos].num_a,"$t0");
    asmStore("$t0",midcodeTab.element[pos].rst);
    pos++;
}

/**
 * jmp
 */ 
void asmJmp()
{
    sprintf(tmp,"\t\tj\t%s\n",midcodeTab.element[pos].rst);
    fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
    pos++;
}

/**
 * lab:
 */ 
void asmLabel()
{
    sprintf(tmp,"%s:\n",midcodeTab.element[pos].rst);
    fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
    pos++;
}

/**
 * printf
 */ 
void asmPrintf()
{
    //printf("false",change[5]);    =>  prtf, false, $_18, int (在globalData的时候已经把false替换为$string)
    if(strcmp(midcodeTab.element[pos].num_a,"") != 0)
    {
        //加载全局变量$string地址
        sprintf(tmp,"\t\tla\t$t0\t%s\n",midcodeTab.element[pos].num_a);
        fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
        //将全部变量地址作为函数参数传递
        sprintf(tmp,"\t\tmove\t$a0\t$t0\n");
        fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
        
        sprintf(tmp,"\t\tli\t$v0\t4\n");
        fwrite(tmp,sizeof(char),strlen(tmp),mipsf);

        //调用系统调用（打印字符串）
        sprintf(tmp,"\t\tsyscall\n");
        fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
    }
    if(strcmp(midcodeTab.element[pos].num_b,"") != 0)
    {
        if(isNum(midcodeTab.element[pos].num_b))
        {
            //li $t0 1
            sprintf(tmp,"\t\tli\t$a0\t%s\n",midcodeTab.element[pos].num_b);
            fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
        }
        else
        {
            //如果var不是数字
            int addr = findInLocalTab(midcodeTab.element[pos].num_b);
            //是否为局部变量
            if(addr != -1)
            {
                //addr是相对$fp来说的
                sprintf(tmp,"\t\tlw\t$a0\t%d($fp)\n",-1*addr);
                fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
            }
            //否则是否为全局变量（优先使用局部变量）
            else if(findInGlobalTab(midcodeTab.element[pos].num_b))
            {
                //la $t0 GLOBAL
                //lw $a0 ($t0)
                sprintf(tmp,"\t\tla\t$t0\t%s\n",midcodeTab.element[pos].num_b);
                fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
                sprintf(tmp,"\t\tlw\t$a0\t($t0)\n");
                fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
            }
            else
            {
                fprintf(stderr,"mips load: '%s\n' don't exist!\n",midcodeTab.element[pos].num_b);
                exit(1);
            }
        }
        //如果是int型 $v0 = 1
        if(strcmp(midcodeTab.element[pos].rst,"int") == 0)
        {
            sprintf(tmp,"\t\tli\t$v0\t1\n");
            fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
        }
        //如果是char型 $v0 = 11
        else if(strcmp(midcodeTab.element[pos].rst,"char") == 0)
        {
            sprintf(tmp,"\t\tli\t$v0\t11\n");
            fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
        }
        sprintf(tmp,"\t\tsyscall\n");
        fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
    }
    pos++;
}

void asmScanf()
{
    //scanf(f)  =>  scf, , int, f
    if(strcmp(midcodeTab.element[pos].num_b,"int") == 0)
    {
        sprintf(tmp,"\t\tli\t$v0\t5\n");
        fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
    }
    else if(strcmp(midcodeTab.element[pos].num_b,"char") == 0)
    {
        sprintf(tmp,"\t\tli\t$v0\t12\n");
        fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
    }
    sprintf(tmp,"\t\tsyscall\n");
    fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
    //scanf函数运行结束后会将输入值存在$v0

    int addr = findInLocalTab(midcodeTab.element[pos].rst);
    //这块好像有问题
    if(addr != -1)
    {
        sprintf(tmp,"\t\tsw\t$v0\t%d($fp)\n",-1*addr);
        fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
    }
    else if(findInGlobalTab(midcodeTab.element[pos].rst))
    {
        sprintf(tmp,"\t\tla\t$t0\t%s\n",midcodeTab.element[pos].rst);
        fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
        sprintf(tmp,"\t\tsw\t$v0\t($t0)\n");
        fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
    }
    pos++;
}

/**
 * return
 */ 
void asmReturn()
{
    //把返回值存储到$v1
    //ret, , , $_4
    if(strcmp(midcodeTab.element[pos].rst,"") != 0)
    {
        if(midcodeTab.element[pos].rst[0] == '-' || 
        isNum(midcodeTab.element[pos].rst))
        {
            sprintf(tmp,"\t\tli\t$v1\t%s\n",midcodeTab.element[pos].rst);
            fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
        }
        else
        {
            int addr = findInLocalTab(midcodeTab.element[pos].rst);
            if(addr != -1)
            {
                sprintf(tmp,"\t\tlw\t$v1\t%d($fp)\n",-1*addr);
                fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
            }
            else if(findInGlobalTab(midcodeTab.element[pos].rst))
            {
                sprintf(tmp,"\t\tla\t$t0\t%s\n",midcodeTab.element[pos].rst);
                fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
                sprintf(tmp,"\t\tlw\t$v1\t($t0)\n");
                fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
            }
            else
            {
                fprintf(stderr,"return: %s don't delcare!\n",midcodeTab.element[pos].rst);
                exit(1);
            }
        }
    }

    //恢复寄存器
    //a调用b，b调用c，c中的$ra是返回b的地址，栈中的ra（也就是b的ra）是返回a的地址
    sprintf(tmp,"\t\tmove\t$t0\t$ra\n");//返回函数b的地址
    fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
    sprintf(tmp,"\t\tlw\t$ra\t-4($fp)\n");//恢复b中的ra
    fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
    sprintf(tmp,"\t\tadd\t$sp\t$fp\t$0\n");//$sp = $fp
    fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
    sprintf(tmp,"\t\tlw\t$fp\t($fp)\n");//$fp = ($fp)
    fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
    sprintf(tmp,"\t\tjr\t$t0\n");//jr $t0
    fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
    pos++;
}

/**
 * end
 */ 
void asmEnd()
{
    printLocalVarTable();
    localTab.len = 0;
    //函数调用
    if(strcmp(midcodeTab.element[pos].rst,"main") != 0)
    {
        //恢复寄存器
        //a调用b，b调用c，c中的$ra是返回b的地址，栈中的ra（也就是b的ra）是返回a的地址
        sprintf(tmp,"\t\tmove\t$t0\t$ra\n");//返回函数b的地址
        fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
        sprintf(tmp,"\t\tlw\t$ra\t-4($fp)\n");//恢复b中的ra
        fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
        sprintf(tmp,"\t\tadd\t$sp\t$fp\t$0\n");//$sp = $fp
        fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
        sprintf(tmp,"\t\tlw\t$fp\t($fp)\n");//$fp = ($fp)
        fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
        sprintf(tmp,"\t\tjr\t$t0\n");//jr $t0
        fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
    }
    //main函数退出
    else
    {
        sprintf(tmp,"\t\tli\t$v0\t10\n");
        fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
        sprintf(tmp,"\t\tsyscall\n");
        fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
    }
    
    pos++;
}

/**
 * 数组赋值
 * 样例：change[0+1]=1; => []=, 1, $_6, change
 */ 
void asmAssignArray()
{
    asmLoad(midcodeTab.element[pos].num_a,"$t0");
    
    asmLoad(midcodeTab.element[pos].num_b,"$t1");
    sprintf(tmp,"\t\tmul\t$t1\t$t1\t-4\n");
    fwrite(tmp,sizeof(char),strlen(tmp),mipsf);

    int addr = findInLocalTab(midcodeTab.element[pos].rst);
    if(addr != -1)//局部变量
    {
        sprintf(tmp,"\t\tli\t$t2\t%d\n",-1*addr);
        fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
        sprintf(tmp,"\t\tadd\t$t1\t$t2\t$t1\n");
        fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
        sprintf(tmp,"\t\tadd\t$t1\t$t1\t$fp\n");
        fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
        sprintf(tmp,"\t\tsw\t$t0\t($t1)\n");
        fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
    }
    else if(findInGlobalTab(midcodeTab.element[pos].rst))
    {
        sprintf(tmp,"\t\tla\t$t2\t%s\n",midcodeTab.element[pos].rst);
        fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
        sprintf(tmp,"\t\tadd\t$t1\t$t2\t$t1\n");
        fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
        sprintf(tmp,"\t\tsw\t$t0\t($t1)\n");
        fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
    }
    
    pos++;
}

/**
 * 把数组赋值给变量
 * 样例：x = change[1+0] => aassi, change, $_9, $_10 (+, 1, 0, $_9)
 */ 
void asmArrayAssign()
{
    int addr = findInLocalTab(midcodeTab.element[pos].num_a);
    if(addr != -1)//局部
    {
        sprintf(tmp,"\t\tli\t$t0\t%d\n",-1*addr);
        fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
        sprintf(tmp,"\t\tadd\t$t0\t$t0\t$fp");
        fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
    }
    else if(findInGlobalTab(midcodeTab.element[pos].num_a))
    {
        sprintf(tmp,"\t\tla\t$t0\t%s\n",midcodeTab.element[pos].num_a);
        fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
    }

    asmLoad(midcodeTab.element[pos].num_b,"$t1");
    sprintf(tmp,"\t\tmul\t$t1\t$t1\t-4\n");
    fwrite(tmp,sizeof(char),strlen(tmp),mipsf);

    sprintf(tmp,"\t\tadd\t$t0\t$t0\t$t1\n");
    fwrite(tmp,sizeof(char),strlen(tmp),mipsf);
    sprintf(tmp,"\t\tlw\t$t0\t($t0)\n");
    fwrite(tmp,sizeof(char),strlen(tmp),mipsf);

    insertAddr(midcodeTab.element[pos].rst);
    addr = findInLocalTab(midcodeTab.element[pos].rst);
    sprintf(tmp,"\t\tsw\t$t0\t%d($fp)\n",-1*addr);
    fwrite(tmp,sizeof(char),strlen(tmp),mipsf);

    pos++;
}

void asmRun()
{
    midcodeTab = getMidCode();
    //程序设定：先定义全局变量、然后是程序
    mipsf = fopen("result/mips.txt","w");
    asmGlobalData();
    
    while(pos < midcodeTab.length)
    {
        if(strcmp(midcodeTab.element[pos].op,"func") == 0)//如果是函数func, , , main
        {
            asmFunc();
        }
        else if(strcmp(midcodeTab.element[pos].op,"para") == 0)//para, int, , a
        {
            asmParam();
        }
        else if(strcmp(midcodeTab.element[pos].op,"inta") == 0 || 
            strcmp(midcodeTab.element[pos].op,"chara") == 0 ||
            strcmp(midcodeTab.element[pos].op,"char") == 0 ||
            strcmp(midcodeTab.element[pos].op,"int") == 0)
        {
            asmVarDeclare();
        }
        else if(strcmp(midcodeTab.element[pos].op,">") == 0)
        {
            asmBt();
        }
        else if(strcmp(midcodeTab.element[pos].op,">=") == 0)
        {
            asmNst();
        }
        else if(strcmp(midcodeTab.element[pos].op,"<") == 0)
        {
            asmSt();
        }
        else if(strcmp(midcodeTab.element[pos].op,"<=") == 0)
        {
            asmNbt();
        }
        else if(strcmp(midcodeTab.element[pos].op,"==") == 0)
        {
            asmEq();
        }
        else if(strcmp(midcodeTab.element[pos].op,"!=") == 0)
        {
            asmNeq();
        }
        else if(strcmp(midcodeTab.element[pos].op,"jne") == 0)
        {
            asmJumpToLable();
        }
        else if(strcmp(midcodeTab.element[pos].op,"+") == 0)
        {
            asmAdd();
        }
        else if(strcmp(midcodeTab.element[pos].op,"-") == 0)
        {
            asmSub();
        }
        else if(strcmp(midcodeTab.element[pos].op,"*") == 0)
        {
            asmMul();
        }
        else if(strcmp(midcodeTab.element[pos].op,"/") == 0)
        {
            asmDiv();
        }
        else if(strcmp(midcodeTab.element[pos].op,"=") == 0)
        {
            asmAssign();
        }
        else if(strcmp(midcodeTab.element[pos].op,"[]=") == 0)
        {
            asmAssignArray();
        }
        else if(strcmp(midcodeTab.element[pos].op,"aassi") == 0)
        {
            asmArrayAssign();
        }
        else if(strcmp(midcodeTab.element[pos].op,"jmp") == 0)
        {
            asmJmp();
        }
        else if(strcmp(midcodeTab.element[pos].op,"lab:") == 0)
        {
            asmLabel();
        }
        else if(strcmp(midcodeTab.element[pos].op,"prtf") == 0)
        {
            asmPrintf();
        }
        else if(strcmp(midcodeTab.element[pos].op,"scf") == 0)
        {
            asmScanf();
        }
        else if(strcmp(midcodeTab.element[pos].op,"ret") == 0)
        {
            asmReturn();
        }
        else if(strcmp(midcodeTab.element[pos].op,"end") == 0)
        {
            asmEnd();
        }
        else if(strcmp(midcodeTab.element[pos].op,"call") == 0)
        {
            asmCall();
        }
        else if(strcmp(midcodeTab.element[pos].op,"cpara") == 0)
        {
            asmCallParam();
        }
    }

    printGlobalTable();
    printLocalVarTable();
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
    printf("------------------fun: %s var table------------------\n",midcodeTab.element[pos].rst);
    for(int i =0; i < localTab.len; i++)
    {
        printf("name: %s\t\taddr: %d\n",localTab.l[i].name,localTab.l[i].addr);
    }
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
