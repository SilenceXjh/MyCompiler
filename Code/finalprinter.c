#include "finalprinter.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

extern int offset;

char *getReg(int regNum)
{
    assert(regNum <= 21);
    char* res = (char*)malloc(sizeof(char) * 4);
    res[0] = '$';
    res[3] = '\0';
    if (regNum <= 9)
    {
        res[1] = 't';
        res[2] = (char)(regNum + '0');
    }
    else if (regNum <= 17)
    {
        res[1] = 's';
        res[2] = (char)(regNum - 10 + '0');
    }
    else
    {
        res[1] = 'a';
        res[2] = (char)(regNum - 18 + '0');
    }
    return res;
}

void printSpill(FILE *fp, int regNum)
{
    if (regNum <= 9)
    {
        fprintf(fp, "sw $t%d, %d($sp)\n", regNum, offset);
    }
    else if (regNum <= 17)
    {
        fprintf(fp, "sw $s%d, %d($sp)\n", regNum - 10, offset);
    }
}

void printLi(FILE *fp, int regNum, int imm)
{
    fprintf(fp, "li %s, %d\n", getReg(regNum), imm);
}

void printLa(FILE *fp, int regNum, int off)
{
    fprintf(fp, "la %s, %d($sp)\n", getReg(regNum), off);
}

void printMove(FILE *fp, int regNum1, int regNum2)
{
    fprintf(fp, "move %s, %s\n", getReg(regNum1), getReg(regNum2));
}

void printAdd(FILE *fp, int regNum1, int regNum2, int regNum3)
{
    fprintf(fp, "add %s, %s, %s\n", getReg(regNum1), getReg(regNum2), getReg(regNum3));
}

void printSub(FILE *fp, int regNum1, int regNum2, int regNum3)
{
    fprintf(fp, "sub %s, %s, %s\n", getReg(regNum1), getReg(regNum2), getReg(regNum3));
}

void printMul(FILE *fp, int regNum1, int regNum2, int regNum3)
{
    fprintf(fp, "mul %s, %s, %s\n", getReg(regNum1), getReg(regNum2), getReg(regNum3));
}

void printMuli(FILE *fp, int regNum1, int regNum2, int imm)
{
    fprintf(fp, "mul %s, %s, %d\n", getReg(regNum1), getReg(regNum2), imm);
}

void printDiv(FILE *fp, int regNum1, int regNum2, int regNum3)
{
    fprintf(fp, "div %s, %s\n", getReg(regNum2), getReg(regNum3));
    fprintf(fp, "mflo %s\n", getReg(regNum1));
}

void printIfgo(FILE *fp, int r1, int r2, char *relop, int label_no)
{
    char order[4];
    if (strcmp(relop, "==") == 0)
    {
        strcpy(order, "beq");
    }
    else if (strcmp(relop, "!=") == 0)
    {
        strcpy(order, "bne");
    }
    else if (strcmp(relop, ">") == 0)
    {
        strcpy(order, "bgt");
    }
    else if (strcmp(relop, "<") == 0)
    {
        strcpy(order, "blt");
    }
    else if (strcmp(relop, ">=") == 0)
    {
        strcpy(order, "bge");
    }
    else if (strcmp(relop, "<=") == 0)
    {
        strcpy(order, "ble");
    }
    fprintf(fp, "%s %s, %s, label%d\n", order, getReg(r1), getReg(r2), label_no);
}

void printReturn(FILE* fp, int regNum, int frameSize) {
    fprintf(fp, "move $v0, %s\n", getReg(regNum));
    fprintf(fp, "addi $sp, $sp, %d\n", frameSize);
    fprintf(fp, "jr $ra\n");
}