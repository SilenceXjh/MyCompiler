#include <stdio.h>

char *getReg(int regNum);

void printSpill(FILE *fp, int regNum);

void printLi(FILE* fp, int regNum, int imm);

void printLa(FILE* fp, int regNum, int off);

void printMove(FILE* fp, int regNum1, int regNum2);

void printAdd(FILE* fp, int regNum1, int regNum2, int regNum3);

void printSub(FILE* fp, int regNum1, int regNum2, int regNum3);

void printMul(FILE* fp, int regNum1, int regNum2, int regNum3);

void printMuli(FILE* fp, int regNum1, int regNum2, int imm);

void printDiv(FILE* fp, int regNum1, int regNum2, int regNum3);

void printIfgo(FILE* fp, int r1, int r2, char* relop, int label_no);

void printReturn(FILE* fp, int regNum, int frameSize);

