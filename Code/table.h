#ifndef _TABLE_H_
#define _TABLE_H_

#include "tree.h"

typedef struct Type_* Type;
typedef struct FieldList_* FieldList;
typedef struct Record_* Record;
typedef struct Var_* Var;
typedef struct Func_* Func;

struct Type_ {
    enum { BASIC, ARRAY, STRUCTURE } kind;
    union 
    {
        int basic;
        struct { Type elem; int size; } array;
        int stru_index;
    }u;
    int weight;
    int isParam;
};

struct FieldList_ 
{
    char name[34];
    Type type;
    FieldList tail;
};

struct Record_ {
    int index;
    char name[34];
    FieldList fieldlist;
    Record next;
};

struct Var_ {
    char name[34];
    Type t;
    Var next;
    int index;
};

struct Func_ {
    char name[34];
    Type retType;
    FieldList params;
    Func next;
};

int query_record(char* _name);

unsigned int hash_pjw(char* name);

Type getVarType(char* _name);

int getVarIndex(char* _name);

FieldList getFuncParams(char* _name);

Type getFuncRet(char* _name);

int getTypeSize(Type t);

void fillin_vartable_global(char* _name, Type _t, int _lineno);

int typeEqual(Type t1, Type t2);

void insertR();

void insertW();

#endif