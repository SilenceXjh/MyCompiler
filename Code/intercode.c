#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "intercode.h"

Intercodes head = NULL;

int tmpNum = 0;
int labelNum = 0;

Operand genVariable(int no)
{
    Operand op = (Operand)malloc(sizeof(struct Operand_));
    op->kind = VARIABLE;
    op->u.var_no = no;
    return op;
}

Operand genTemp()
{
    Operand op = (Operand)malloc(sizeof(struct Operand_));
    op->kind = TEMP;
    op->u.tmp_no = tmpNum++;
    return op;
}

Operand genInt(int value)
{
    Operand op = (Operand)malloc(sizeof(struct Operand_));
    op->kind = CONSTANTINT;
    op->u.valueInt = value;
    return op;
}

Operand genLabel()
{
    Operand op = (Operand)malloc(sizeof(struct Operand_));
    op->kind = LABEL;
    op->u.lable_no = labelNum++;
    return op;
}

Arg genArg(Operand _op)
{
    Arg arg = (Arg)malloc(sizeof(struct Arg_));
    arg->op = _op;
    arg->next = NULL;
    return arg;
}

Operand genAddr(int no)
{
    Operand op = (Operand)malloc(sizeof(struct Operand_));
    op->kind = ADDR;
    op->u.var_no = no;
    return op;
}

void insertCode(Intercodes code)
{
    if (code == NULL)
        return;
    if (head == NULL)
    {
        head = code;
        return;
    }
    Intercodes tail = head->prev;
    Intercodes cTail = code->prev;
    tail->next = code;
    code->prev = tail;
    cTail->next = head;
    head->prev = cTail;
}

Intercodes mergeCode(Intercodes code1, Intercodes code2)
{
    if (code1 == NULL)
        return code2;
    if (code2 == NULL)
        return code1;
    Intercodes tail1 = code1->prev;
    Intercodes tail2 = code2->prev;
    tail1->next = code2;
    code2->prev = tail1;
    tail2->next = code1;
    code1->prev = tail2;
    return code1;
}

Intercodes genCodes(Intercode c)
{
    Intercodes codes = (Intercodes)malloc(sizeof(struct Intercode_));
    codes->code = c;
    codes->next = codes;
    codes->prev = codes;
    return codes;
}

Intercodes genFunDec(char *name)
{
    Operand op = (Operand)malloc(sizeof(struct Operand_));
    op->kind = FUNC;
    op->u.func_name = (char *)malloc(strlen(name) + 1);
    strcpy(op->u.func_name, name);
    Intercode c = (Intercode)malloc(sizeof(struct Intercode_));
    c->kind = FUNDEC;
    c->u.singleop.op0 = op;
    return genCodes(c);
}

Intercodes genParam(char *name)
{
    int no = getVarIndex(name);
    Operand op = genVariable(no);
    Intercode c = (Intercode)malloc(sizeof(struct Intercode_));
    c->kind = PARAM;
    c->u.singleop.op0 = op;
    return genCodes(c);
}

Intercodes genArrayDec(char *name, int _size)
{
    int no = getVarIndex(name);
    Operand op = genVariable(no);
    Intercode c = (Intercode)malloc(sizeof(struct Intercode_));
    c->kind = ARRAYDEC;
    c->u.decsize.op0 = op;
    c->u.decsize.size = _size;
    return genCodes(c);
}

Intercodes genAssign(Operand _left, Operand _right)
{
    Intercode c = (Intercode)malloc(sizeof(struct Intercode_));
    c->kind = ASSIGN;
    c->u.assign.left = _left;
    c->u.assign.right = _right;
    return genCodes(c);
}

Intercodes genAdd(Operand _result, Operand _op1, Operand _op2)
{
    Intercode c = (Intercode)malloc(sizeof(struct Intercode_));
    c->kind = ADD;
    c->u.binop.result = _result;
    c->u.binop.op1 = _op1;
    c->u.binop.op2 = _op2;
    return genCodes(c);
}

Intercodes genSub(Operand _result, Operand _op1, Operand _op2)
{
    Intercode c = (Intercode)malloc(sizeof(struct Intercode_));
    c->kind = SUB;
    c->u.binop.result = _result;
    c->u.binop.op1 = _op1;
    c->u.binop.op2 = _op2;
    return genCodes(c);
}

Intercodes genMul(Operand _result, Operand _op1, Operand _op2)
{
    Intercode c = (Intercode)malloc(sizeof(struct Intercode_));
    c->kind = MUL;
    c->u.binop.result = _result;
    c->u.binop.op1 = _op1;
    c->u.binop.op2 = _op2;
    return genCodes(c);
}

Intercodes genDiv(Operand _result, Operand _op1, Operand _op2)
{
    Intercode c = (Intercode)malloc(sizeof(struct Intercode_));
    c->kind = DIV;
    c->u.binop.result = _result;
    c->u.binop.op1 = _op1;
    c->u.binop.op2 = _op2;
    return genCodes(c);
}

Intercodes genLabelDec(Operand label)
{
    Intercode c = (Intercode)malloc(sizeof(struct Intercode_));
    c->kind = LABELDEC;
    c->u.singleop.op0 = label;
    return genCodes(c);
}

Intercodes genIfGoto(Operand t1, Operand t2, char *rel, Operand _label)
{
    Intercode c = (Intercode)malloc(sizeof(struct Intercode_));
    c->kind = IFGOTO;
    c->u.ifgoto.label = _label;
    c->u.ifgoto.op1 = t1;
    c->u.ifgoto.op2 = t2;
    strcpy(c->u.ifgoto.relop, rel);
    return genCodes(c);
}

Intercodes genGoto(Operand label)
{
    Intercode c = (Intercode)malloc(sizeof(struct Intercode_));
    c->kind = GOTO;
    c->u.singleop.op0 = label;
    return genCodes(c);
}

Intercodes genReturn(Operand ret)
{
    Intercode c = (Intercode)malloc(sizeof(struct Intercode_));
    c->kind = RETURN;
    c->u.singleop.op0 = ret;
    return genCodes(c);
}

Intercodes genRead(Operand op)
{
    Intercode c = (Intercode)malloc(sizeof(struct Intercode_));
    c->kind = READ;
    c->u.singleop.op0 = op;
    return genCodes(c);
}

Intercodes genCall(Operand _left, char *name)
{
    Intercode c = (Intercode)malloc(sizeof(struct Intercode_));
    c->kind = CALL;
    c->u.call.left = _left;
    c->u.call.func_name = (char *)malloc(strlen(name) + 1);
    strcpy(c->u.call.func_name, name);
    return genCodes(c);
}

Intercodes genWrite(Operand op)
{
    Intercode c = (Intercode)malloc(sizeof(struct Intercode_));
    c->kind = WRITE;
    c->u.singleop.op0 = op;
    return genCodes(c);
}

Intercodes genArgDec(Arg arg_list)
{
    if (arg_list == NULL)
        return NULL;
    Intercodes code1 = genArgDec(arg_list->next);
    Intercode c = (Intercode)malloc(sizeof(struct Intercode_));
    c->kind = ARG;
    c->u.singleop.op0 = arg_list->op;
    Intercodes code2 = genCodes(c);
    return mergeCode(code1, code2);
}

Intercodes genRightStar(Operand _left, Operand _right)
{
    Intercode c = (Intercode)malloc(sizeof(struct Intercode_));
    c->kind = RIGHTSTAR;
    c->u.assign.left = _left;
    c->u.assign.right = _right;
    return genCodes(c);
}

Intercodes genLeftStar(Operand _left, Operand _right)
{
    Intercode c = (Intercode)malloc(sizeof(struct Intercode_));
    c->kind = LEFTSTAR;
    c->u.assign.left = _left;
    c->u.assign.right = _right;
    return genCodes(c);
}

void printOperand(FILE *fp, Operand op)
{
    if (op == NULL)
        return;
    if (op->kind == VARIABLE)
    {
        fprintf(fp, "v%d", op->u.var_no);
    }
    else if (op->kind == TEMP)
    {
        fprintf(fp, "t%d", op->u.tmp_no);
    }
    else if (op->kind == CONSTANTINT)
    {
        fprintf(fp, "#%d", op->u.valueInt);
    }
    else if (op->kind == FUNC)
    {
        fprintf(fp, "%s", op->u.func_name);
    }
    else if (op->kind == LABEL)
    {
        fprintf(fp, "label%d", op->u.lable_no);
    }
    else if (op->kind == ADDR)
    {
        fprintf(fp, "&v%d", op->u.var_no);
    }
}

void printIntercode(FILE *fp, Intercode c)
{
    if (c->kind == ASSIGN)
    {
        if (c->u.assign.left == NULL || c->u.assign.right == NULL)
        {
            return;
        }
        printOperand(fp, c->u.assign.left);
        fprintf(fp, " := ");
        printOperand(fp, c->u.assign.right);
    }
    else if (c->kind == ADD)
    {
        if (c->u.binop.result == NULL || c->u.binop.op1 == NULL || c->u.binop.op2 == NULL)
        {
            return;
        }
        printOperand(fp, c->u.binop.result);
        fprintf(fp, " := ");
        printOperand(fp, c->u.binop.op1);
        fprintf(fp, " + ");
        printOperand(fp, c->u.binop.op2);
    }
    else if (c->kind == SUB)
    {
        if (c->u.binop.result == NULL || c->u.binop.op1 == NULL || c->u.binop.op2 == NULL)
        {
            return;
        }
        printOperand(fp, c->u.binop.result);
        fprintf(fp, " := ");
        printOperand(fp, c->u.binop.op1);
        fprintf(fp, " - ");
        printOperand(fp, c->u.binop.op2);
    }
    else if (c->kind == MUL)
    {
        if (c->u.binop.result == NULL || c->u.binop.op1 == NULL || c->u.binop.op2 == NULL)
        {
            return;
        }
        printOperand(fp, c->u.binop.result);
        fprintf(fp, " := ");
        printOperand(fp, c->u.binop.op1);
        fprintf(fp, " * ");
        printOperand(fp, c->u.binop.op2);
    }
    else if (c->kind == DIV)
    {
        if (c->u.binop.result == NULL || c->u.binop.op1 == NULL || c->u.binop.op2 == NULL)
        {
            return;
        }
        printOperand(fp, c->u.binop.result);
        fprintf(fp, " := ");
        printOperand(fp, c->u.binop.op1);
        fprintf(fp, " / ");
        printOperand(fp, c->u.binop.op2);
    }
    else if (c->kind == FUNDEC)
    {
        if (c->u.singleop.op0 == NULL)
        {
            return;
        }
        fprintf(fp, "FUNCTION ");
        printOperand(fp, c->u.singleop.op0);
        fprintf(fp, " :");
    }
    else if (c->kind == PARAM)
    {
        if (c->u.singleop.op0 == NULL)
        {
            return;
        }
        fprintf(fp, "PARAM ");
        printOperand(fp, c->u.singleop.op0);
    }
    else if (c->kind == ARRAYDEC)
    {
        if (c->u.decsize.op0 == NULL)
        {
            return;
        }
        fprintf(fp, "DEC ");
        printOperand(fp, c->u.decsize.op0);
        fprintf(fp, " %d ", c->u.decsize.size);
    }
    else if (c->kind == LABELDEC)
    {
        if (c->u.singleop.op0 == NULL)
        {
            return;
        }
        fprintf(fp, "LABEL ");
        printOperand(fp, c->u.singleop.op0);
        fprintf(fp, " :");
    }
    else if (c->kind == IFGOTO)
    {
        if (c->u.ifgoto.label == NULL || c->u.ifgoto.op1 == NULL || c->u.ifgoto.op2 == NULL)
        {
            return;
        }
        fprintf(fp, "IF ");
        printOperand(fp, c->u.ifgoto.op1);
        fprintf(fp, " %s ", c->u.ifgoto.relop);
        printOperand(fp, c->u.ifgoto.op2);
        fprintf(fp, " GOTO ");
        printOperand(fp, c->u.ifgoto.label);
    }
    else if (c->kind == GOTO)
    {
        if (c->u.singleop.op0 == NULL)
        {
            return;
        }
        fprintf(fp, "GOTO ");
        printOperand(fp, c->u.singleop.op0);
    }
    else if (c->kind == RETURN)
    {
        if (c->u.singleop.op0 == NULL)
        {
            return;
        }
        fprintf(fp, "RETURN ");
        printOperand(fp, c->u.singleop.op0);
    }
    else if (c->kind == READ)
    {
        if (c->u.singleop.op0 == NULL)
        {
            return;
        }
        fprintf(fp, "READ ");
        printOperand(fp, c->u.singleop.op0);
    }
    else if (c->kind == WRITE)
    {
        if (c->u.singleop.op0 == NULL)
        {
            return;
        }
        fprintf(fp, "WRITE ");
        printOperand(fp, c->u.singleop.op0);
    }
    else if (c->kind == CALL)
    {
        if (c->u.call.left == NULL)
        {
            return;
        }
        printOperand(fp, c->u.call.left);
        fprintf(fp, " := CALL %s", c->u.call.func_name);
    }
    else if (c->kind == ARG)
    {
        if (c->u.singleop.op0 == NULL)
        {
            return;
        }
        fprintf(fp, "ARG ");
        printOperand(fp, c->u.singleop.op0);
    }
    else if (c->kind == RIGHTSTAR)
    {
        if (c->u.assign.left == NULL || c->u.assign.right == NULL)
        {
            return;
        }
        printOperand(fp, c->u.assign.left);
        fprintf(fp, " := *");
        printOperand(fp, c->u.assign.right);
    }
    else if (c->kind == LEFTSTAR)
    {
        if (c->u.assign.left == NULL || c->u.assign.right == NULL)
        {
            return;
        }
        fprintf(fp, "*");
        printOperand(fp, c->u.assign.left);
        fprintf(fp, " := ");
        printOperand(fp, c->u.assign.right);
    }
    fprintf(fp, "\n");
}

void printIntercodes(FILE *fp)
{
    if (head == NULL)
        return;
    Intercodes p = head;
    printIntercode(fp, p->code);
    p = p->next;
    while (p != head)
    {
        printIntercode(fp, p->code);
        p = p->next;
    }
}