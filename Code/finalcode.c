#include "finalcode.h"

#include "finalprinter.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

extern int varNum;
extern int tmpNum;

extern Intercodes head;

Intercode *intercodes;

int *varLoc;
int *tmpLoc;
Operand regs[18];
Operand a[4];

int offset = 0;

void initialWork(FILE *fp)
{
    fprintf(fp, "%s\n", ".data");
    fprintf(fp, "%s\n", "_prompt: .asciiz \"Enter an integer:\"");
    fprintf(fp, "%s\n", "_ret: .asciiz \"\\n\"");
    fprintf(fp, "%s\n", ".globl main");
    fprintf(fp, "%s\n", ".text");
    fprintf(fp, "%s\n", "read:");
    fprintf(fp, "\t%s\n", "li $v0, 4");
    fprintf(fp, "\t%s\n", "la $a0, _prompt");
    fprintf(fp, "\t%s\n", "syscall");
    fprintf(fp, "\t%s\n", "li $v0, 5");
    fprintf(fp, "\t%s\n", "syscall");
    fprintf(fp, "\t%s\n", "jr $ra");
    fprintf(fp, "%s\n", "write:");
    fprintf(fp, "\t%s\n", "li $v0, 1");
    fprintf(fp, "\t%s\n", "syscall");
    fprintf(fp, "\t%s\n", "li $v0, 4");
    fprintf(fp, "\t%s\n", "la $a0, _ret");
    fprintf(fp, "\t%s\n", "syscall");
    fprintf(fp, "\t%s\n", "move $v0, $0");
    fprintf(fp, "\t%s\n", "jr $ra");
    varLoc = (int *)malloc(sizeof(int) * varNum);
    for (int i = 0; i < varNum; ++i)
    {
        varLoc[i] = -1;
    }
    tmpLoc = (int *)malloc(sizeof(int) * tmpNum);
    for (int i = 0; i < tmpNum; ++i)
    {
        tmpLoc[i] = -1;
    }
    for (int i = 0; i < 18; ++i)
    {
        regs[i] = NULL;
    }
    int count = 1;
    if (emptyCode(head->code))
    {
        count = 0;
    }
    Intercodes p = head->next;
    while (p != head)
    {
        if (!emptyCode(p->code))
        {
            count++;
        }
        p = p->next;
    }
    // printf("count %d\n", count);
    intercodes = (Intercode *)malloc(sizeof(Intercode) * count);
    p = head;
    for (int i = 0; i < count; ++i)
    {
        if (!emptyCode(p->code))
        {
            intercodes[i] = p->code;
        }
        else
        {
            i--;
        }
        p = p->next;
    }
    // assert(p == head);
    int start = 0;
    while (intercodes[start]->kind != FUNDEC)
    {
        start++;
    }
    int end = start + 1;
    while (end < count)
    {
        while (end < count && intercodes[end]->kind != FUNDEC)
        {
            end++;
        }
        // printf("start %d, end %d\n", start, end - 1);
        do_func(fp, start, end - 1);
        start = end;
        end++;
    }
}

void do_func(FILE *fp, int start, int end)
{
    offset = 0;
    fprintf(fp, "%s:\n", intercodes[start]->u.singleop.op0->u.func_name);
    start++;
    int origin = start;
    while (intercodes[start]->kind == PARAM)
    {
        Operand op = intercodes[start]->u.singleop.op0;
        if (start - origin < 4)
        {
            varLoc[op->u.var_no] = 18 + start - origin;
        }
        else
        {
            varLoc[op->u.var_no] = 100 + start - origin;
        }
        start++;
    }
    int frameSize = 0;
    for (int i = start; i <= end; ++i)
    {
        if (intercodes[i]->kind == ARRAYDEC)
        {
            frameSize += intercodes[i]->u.decsize.size;
        }
    }
    frameSize += 10000;
    fprintf(fp, "addi $sp, $sp, -%d\n", frameSize);
    int s = start;
    int e = start + 1;
    while (e <= end)
    {
        if (intercodes[e]->kind == LABELDEC)
        {
            do_block(fp, s, e - 1, frameSize);
            s = e;
            e++;
        }
        else if (intercodes[e]->kind == IFGOTO || intercodes[e]->kind == GOTO)
        {
            do_block(fp, s, e, frameSize);
            s = e + 1;
            e = s + 1;
        }
        else
        {
            e++;
        }
    }
    if (s <= end)
    {
        do_block(fp, s, end, frameSize);
    }
}

void do_block(FILE *fp, int start, int end, int frameSize)
{
    for (int i = start; i <= end; ++i)
    {
        do_row(fp, intercodes[i], i, end, frameSize);
    }
}

void do_row(FILE *fp, Intercode code, int cur, int end, int frameSize)
{
    assert(code != NULL);
    if (code->kind == ASSIGN)
    {
        Operand op1 = code->u.assign.left;
        Operand op2 = code->u.assign.right;
        assert(op1 != NULL && op2 != NULL);
        int r1 = findReg(fp, op1, cur, end, 0);
        if (op2->kind == CONSTANTINT)
        {
            printLi(fp, r1, op2->u.valueInt);
        }
        else if (op2->kind == VARIABLE || op2->kind == TEMP)
        {
            int r2 = findReg(fp, op2, cur, end, 1);
            printMove(fp, r1, r2);
        }
        else if (op2->kind == ADDR)
        {
            int off = varLoc[op2->u.var_no];
            printLa(fp, r1, off);
        }
    }
    else if (code->kind == ADD)
    {
        int r1 = findReg(fp, code->u.binop.result, cur, end, 0);
        int r2 = findReg(fp, code->u.binop.op1, cur, end, 1);
        int r3 = findReg(fp, code->u.binop.op2, cur, end, 1);
        printAdd(fp, r1, r2, r3);
    }
    else if (code->kind == SUB)
    {
        int r1 = findReg(fp, code->u.binop.result, cur, end, 0);
        int r2 = findReg(fp, code->u.binop.op1, cur, end, 1);
        int r3 = findReg(fp, code->u.binop.op2, cur, end, 1);
        printSub(fp, r1, r2, r3);
    }
    else if (code->kind == MUL)
    {
        int r1 = findReg(fp, code->u.binop.result, cur, end, 0);
        int r2 = findReg(fp, code->u.binop.op1, cur, end, 1);
        if (code->u.binop.op2->kind == VARIABLE || code->u.binop.op2->kind == TEMP)
        {
            int r3 = findReg(fp, code->u.binop.op2, cur, end, 1);
            printMul(fp, r1, r2, r3);
        }
        else
        {
            printMuli(fp, r1, r2, code->u.binop.op2->u.valueInt);
        }
    }
    else if (code->kind == DIV)
    {
        int r1 = findReg(fp, code->u.binop.result, cur, end, 0);
        int r2 = findReg(fp, code->u.binop.op1, cur, end, 1);
        int r3 = findReg(fp, code->u.binop.op2, cur, end, 1);
        printDiv(fp, r1, r2, r3);
    }
    else if (code->kind == ARRAYDEC)
    {
        Operand arr = code->u.decsize.op0;
        varLoc[arr->u.var_no] = 200 + offset;
        offset += code->u.decsize.size;
    }
    else if (code->kind == LABELDEC)
    {
        fprintf(fp, "label%d:\n", code->u.singleop.op0->u.lable_no);
    }
    else if (code->kind == IFGOTO)
    {
        int r1 = findReg(fp, code->u.ifgoto.op1, cur, end, 1);
        int r2 = findReg(fp, code->u.ifgoto.op2, cur, end, 1);
        printIfgo(fp, r1, r2, code->u.ifgoto.relop, code->u.ifgoto.label->u.lable_no);
    }
    else if (code->kind == GOTO)
    {
        fprintf(fp, "j label%d\n", code->u.singleop.op0->u.lable_no);
    }
    else if (code->kind == RETURN)
    {
        int r1 = findReg(fp, code->u.singleop.op0, cur, end, 1);
        printReturn(fp, r1, frameSize);
    }
    else if (code->kind == READ)
    {
        int r1 = findReg(fp, code->u.singleop.op0, cur, end, 0);
        fprintf(fp, "sw $ra, %d($sp)\n", offset);
        fprintf(fp, "jal read\n");
        fprintf(fp, "lw $ra, %d($sp)\n", offset);
        fprintf(fp, "move %s, $v0\n", getReg(r1));
    }
    else if (code->kind == WRITE)
    {
        int r1 = findReg(fp, code->u.singleop.op0, cur, end, 1);
        fprintf(fp, "move $a0, %s\n", getReg(r1));
        fprintf(fp, "sw $ra, %d($sp)\n", offset);
        fprintf(fp, "jal write\n");
        fprintf(fp, "lw $ra, %d($sp)\n", offset);
    }
    else if (code->kind == CALL)
    {
        Operand save[18];
        for (int i = 0; i < 18; ++i)
        {
            save[i] = NULL;
        }
        for (int i = 0; i < 18; ++i)
        {
            if (regs[i] != NULL)
            {
                fprintf(fp, "sw %s, %d($sp)\n", getReg(i), offset);
                Operand op = regs[i];
                if (op->kind == VARIABLE)
                {
                    varLoc[op->u.var_no] = 200 + offset;
                }
                else if (op->kind == TEMP)
                {
                    tmpLoc[op->u.tmp_no] = 200 + offset;
                }
                save[i] = op;
                regs[i] = NULL;
                offset += 4;
            }
        }
        fprintf(fp, "sw $ra, %d($sp)\n", offset);
        fprintf(fp, "jal %s\n", code->u.call.func_name);
        fprintf(fp, "lw $ra, %d($sp)\n", offset);
        for (int i = 0; i < 18; ++i)
        {
            if (save[i] != NULL)
            {
                Operand op = save[i];
                int pos = 0;
                if (op->kind == VARIABLE)
                {
                    pos = varLoc[op->u.var_no] - 200;
                    varLoc[op->u.var_no] = i;
                }
                else if (op->kind == TEMP)
                {
                    pos = tmpLoc[op->u.tmp_no] - 200;
                    tmpLoc[op->u.tmp_no] = i;
                }
                fprintf(fp, "lw %s, %d($sp)\n", getReg(i), pos);
                regs[i] = op;
            }
        }
        int r1 = findReg(fp, code->u.call.left, cur, end, 0);
        fprintf(fp, "move %s, $v0\n", getReg(r1));
    }
    else if (code->kind == ARG)
    {
        Operand arg = code->u.singleop.op0;
        int r1 = findReg(fp, arg, cur, end, 1);
        int index = 1;
        int p = cur + 1;
        while (p <= end)
        {
            if (intercodes[p]->kind == ARG)
            {
                index++;
                p++;
            }
            else
            {
                break;
            }
        }
        if (index <= 4)
        {
            printMove(fp, 17 + index, r1);
        }
    }
    else if (code->kind == RIGHTSTAR)
    {
        int r1 = findReg(fp, code->u.assign.left, cur, end, 0);
        int r2 = findReg(fp, code->u.assign.right, cur, end, 1);
        fprintf(fp, "lw %s,  0(%s)\n", getReg(r1), getReg(r2));
    }
    else if (code->kind == LEFTSTAR)
    {
        int r1 = findReg(fp, code->u.assign.left, cur, end, 1);
        int r2 = findReg(fp, code->u.assign.right, cur, end, 1);
        fprintf(fp, "sw %s,  0(%s)\n", getReg(r2), getReg(r1));
    }
}

int findReg(FILE *fp, Operand op, int cur, int end, int use)
{
    assert(op != NULL);
    if (op->kind == VARIABLE)
    {
        if (varLoc[op->u.var_no] >= 0 && varLoc[op->u.var_no] <= 21)
        {
            return varLoc[op->u.var_no];
        }
    }
    if (op->kind == TEMP)
    {
        if (tmpLoc[op->u.tmp_no] >= 0 && tmpLoc[op->u.tmp_no] <= 21)
        {
            return tmpLoc[op->u.tmp_no];
        }
    }
    for (int i = 0; i < 18; ++i)
    {
        if (regs[i] == NULL)
        {
            regs[i] = op;
            if (op->kind == VARIABLE)
            {
                if (use && varLoc[op->u.var_no] >= 200)
                {
                    fprintf(fp, "lw %s, %d($sp)\n", getReg(i), varLoc[op->u.var_no] - 200);
                }
                varLoc[op->u.var_no] = i;
                // printf("v%d in reg %d\n", op->u.var_no, i);
            }
            else if (op->kind == TEMP)
            {
                if (use && tmpLoc[op->u.tmp_no] >= 200)
                {
                    fprintf(fp, "lw %s, %d($sp)\n", getReg(i), tmpLoc[op->u.tmp_no] - 200);
                }
                tmpLoc[op->u.tmp_no] = i;
                // printf("t%d in reg %d\n", op->u.tmp_no, i);
            }
            return i;
        }
    }
    int nextUse[18] = {0};
    for (int i = 0; i < 18; ++i)
    {
        Operand inplace = regs[i];
        for (int j = cur; j <= end; ++j)
        {
            if (opInCode(inplace, intercodes[j]))
            {
                nextUse[i] = j;
                break;
            }
        }
        if (nextUse[i] == 0)
        {
            printSpill(fp, i);
            if (inplace->kind == VARIABLE)
            {
                varLoc[inplace->u.var_no] = 200 + offset;
            }
            else if (inplace->kind == TEMP)
            {
                tmpLoc[inplace->u.tmp_no] = 200 + offset;
            }
            offset += 4;
            regs[i] = op;
            if (op->kind == VARIABLE)
            {
                if (use && varLoc[op->u.var_no] >= 200)
                {
                    fprintf(fp, "lw %s, %d($sp)\n", getReg(i), varLoc[op->u.var_no] - 200);
                }
                varLoc[op->u.var_no] = i;
            }
            else if (op->kind == TEMP)
            {
                if (use && tmpLoc[op->u.tmp_no] >= 200)
                {
                    fprintf(fp, "lw %s, %d($sp)\n", getReg(i), tmpLoc[op->u.tmp_no] - 200);
                }
                tmpLoc[op->u.tmp_no] = i;
            }
            return i;
        }
    }
    int choose = 0;
    for (int i = 1; i < 18; ++i)
    {
        if (nextUse[i] > nextUse[choose])
        {
            choose = i;
        }
    }
    Operand inplace = regs[choose];
    printSpill(fp, choose);
    if (inplace->kind == VARIABLE)
    {
        varLoc[inplace->u.var_no] = 200 + offset;
    }
    else if (inplace->kind == TEMP)
    {
        tmpLoc[inplace->u.tmp_no] = 200 + offset;
    }
    offset += 4;
    regs[choose] = op;
    if (op->kind == VARIABLE)
    {
        if (use && varLoc[op->u.var_no] >= 200)
        {
            fprintf(fp, "lw %s, %d($sp)\n", getReg(choose), varLoc[op->u.var_no] - 200);
        }
        varLoc[op->u.var_no] = choose;
    }
    else
    {
        if (use && tmpLoc[op->u.tmp_no] >= 200)
        {
            fprintf(fp, "lw %s, %d($sp)\n", getReg(choose), tmpLoc[op->u.tmp_no] - 200);
        }
        tmpLoc[op->u.tmp_no] = choose;
    }
    return choose;
}

int opInCode(Operand op, Intercode code)
{
    if (code->kind == ASSIGN || code->kind == LEFTSTAR || code->kind == RIGHTSTAR)
    {
        return opEquals(op, code->u.assign.left) || opEquals(op, code->u.assign.right);
    }
    if (code->kind == ADD || code->kind == SUB || code->kind == MUL || code->kind == DIV)
    {
        return opEquals(op, code->u.binop.result) || opEquals(op, code->u.binop.op1) || opEquals(op, code->u.binop.op2);
    }
    if (code->kind == IFGOTO)
    {
        return opEquals(op, code->u.ifgoto.op1) || opEquals(op, code->u.ifgoto.op2);
    }
    if (code->kind == RETURN || code->kind == ARG)
    {
        return opEquals(op, code->u.singleop.op0);
    }
    if (code->kind == READ || WRITE)
    {
        return opEquals(op, code->u.singleop.op0);
    }
    if (code->kind == CALL)
    {
        return opEquals(op, code->u.call.left);
    }
    return 0;
}

int opEquals(Operand op1, Operand op2)
{
    if (op1->kind != op2->kind)
    {
        return 0;
    }
    if (op1->kind == VARIABLE)
    {
        return op1->u.var_no == op2->u.var_no;
    }
    if (op1->kind == TEMP)
    {
        return op1->u.tmp_no == op2->u.tmp_no;
    }
    return 0;
}

int emptyCode(Intercode code)
{
    if (code == NULL)
    {
        return 1;
    }
    if (code->kind == ASSIGN || code->kind == RIGHTSTAR || code->kind == LEFTSTAR)
    {
        return code->u.assign.left == NULL || code->u.assign.right == NULL;
    }
    if (code->kind == ADD || code->kind == SUB || code->kind == MUL || code->kind == DIV)
    {
        return code->u.binop.result == NULL || code->u.binop.op1 == NULL || code->u.binop.op2 == NULL;
    }
    if (code->kind == READ || code->kind == WRITE || code->kind == RETURN ||
        code->kind == CALL || code->kind == ARG)
    {
        return code->u.singleop.op0 == NULL;
    }
    return 0;
}