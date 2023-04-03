#ifndef _INTERCODE_H_
#define _INTERCODE_H_

#include "table.h"
#include <stdio.h>

typedef struct Operand_* Operand;
typedef struct Intercode_* Intercode;
typedef struct Intercodes_* Intercodes;
typedef struct Arg_* Arg;

struct Operand_ {
    enum { VARIABLE, TEMP, CONSTANTINT, FUNC, LABEL, ADDR } kind;
    union {
        int var_no;
        int tmp_no;
        int lable_no;
        int valueInt;
        char* func_name;
    }u;
};

struct Arg_ {
    Operand op;
    Arg next;
};

struct Intercode_ {
    enum { ASSIGN, ADD, SUB, MUL, DIV, FUNDEC, PARAM, ARRAYDEC, LABELDEC, IFGOTO, GOTO, RETURN, 
        READ, WRITE, CALL, ARG, RIGHTSTAR, LEFTSTAR } kind;
    union {
        struct { Operand right, left; } assign;
        struct { Operand result, op1, op2; } binop;
        struct { Operand op0; } singleop;
        struct { Operand op0; int size; } decsize;
        struct { Operand op1; Operand op2; char relop[3]; Operand label; } ifgoto;
        struct { Operand left; char* func_name; } call;
    }u;
};

struct Intercodes_ {
    Intercode code;
    Intercodes prev;
    Intercodes next;
};

Operand genVariable(int no);

Operand genTemp();

Operand genInt(int value);

Operand genLabel();

Operand genAddr(int no);

Arg genArg(Operand _op);

void insertCode(Intercodes code);

Intercodes mergeCode(Intercodes code1, Intercodes code2);

Intercodes genFunDec(char* name);

Intercodes genParam(char* name);

Intercodes genArrayDec(char* name, int _size);

Intercodes genAssign(Operand _left, Operand _right);

Intercodes genAdd(Operand _result, Operand _op1, Operand _op2);

Intercodes genSub(Operand _result, Operand _op1, Operand _op2);

Intercodes genMul(Operand _result, Operand _op1, Operand _op2);

Intercodes genDiv(Operand _result, Operand _op1, Operand _op2);

Intercodes genLabelDec(Operand label);

Intercodes genIfGoto(Operand t1, Operand t2, char* _rel, Operand label);

Intercodes genGoto(Operand label);

Intercodes genReturn(Operand ret);

Intercodes genRead(Operand op);

Intercodes genCall(Operand _left, char* name);

Intercodes genWrite(Operand op);

Intercodes genArgDec(Arg arg_list);

Intercodes genRightStar(Operand _left, Operand _right);

Intercodes genLeftStar(Operand _left, Operand _right);

void printOperand(FILE* fp, Operand op);

void printIntercode(FILE* fp, Intercode c);

void printIntercodes(FILE* fp);

#endif