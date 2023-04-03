#include "intercode.h"

void initialWork(FILE* fp);

void do_func(FILE *fp, int start, int end);

void do_block(FILE *fp, int start, int end, int frameSize);

void do_row(FILE *fp, Intercode code, int cur, int end, int frameSize);

int findReg(FILE *fp, Operand op, int cur, int end, int use);

int opInCode(Operand op, Intercode code);

int opEquals(Operand op1, Operand op2);

int emptyCode(Intercode code);