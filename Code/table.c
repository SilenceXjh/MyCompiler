#include "table.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

Record recordList = NULL;
int record_count = 0;

Var varTable[1024];
Func funcTable[1024];
int varNum = 0;

FieldList curParams = NULL;

int query_record(char* _name) {
    Record p = recordList;
    while(p) {
        if(strcmp(p->name, _name) == 0) {
            return p->index;
        }
        p = p->next;
    }
    return -1;
} 

unsigned int hash_pjw(char* name) {
    unsigned int val = 0, i;
    for( ; *name; ++name) {
        val = (val << 2) + *name;
        if(i = val & ~0x3ff) 
            val = (val ^ (i >> 8)) & 0x3ff;
    }
    return val;
}

Type getVarType(char* _name) {
    int pos = (int)hash_pjw(_name);
    Var p = varTable[pos];
    while(p) {
        if(strcmp(p->name, _name) == 0) {
            return p->t;
        }
        p = p->next;
    }
    return NULL;
}

int getVarIndex(char* _name) {
    int pos = (int)hash_pjw(_name);
    Var p = varTable[pos];
    while(p) {
        if(strcmp(p->name, _name) == 0) {
            return p->index;
        }
        p = p->next;
    }
    return -1;
}

FieldList getFuncParams(char* _name) {
    int pos = (int)hash_pjw(_name);
    Func p = funcTable[pos];
    while(p) {
        if(strcmp(p->name, _name) == 0) {
            return p->params;
        }
        p = p->next;
    }
    return NULL;
}

Type getFuncRet(char* _name) {
    int pos = (int)hash_pjw(_name);
    Func p = funcTable[pos];
    while(p) {
        if(strcmp(p->name, _name) == 0) {
            return p->retType;
        }
        p = p->next;
    }
    return NULL;
}

int getTypeSize(Type t) {
    if(t->kind == BASIC) {
        return 4;
    }
    if(t->kind == ARRAY) {
        return t->u.array.size * getTypeSize(t->u.array.elem);
    }
    return 0;
}

void fillin_vartable_global(char* _name, Type _t, int _lineno) {
    Var newVar = (Var)malloc(sizeof(struct Var_));
    newVar->t = _t;
    newVar->next = NULL;
    strcpy(newVar->name, _name);
    int pos = (int)hash_pjw(_name);
    if(varTable[pos] == NULL) {
        varTable[pos] = newVar;
    }
    else {
        Var p = varTable[pos];
        while(p) {
            if(strcmp(p->name, _name) == 0) {
                printf("Error type 3 at Line %d: Redefined variable %s\n", _lineno, _name);
                free(newVar);
                return;
            }
            p = p->next;
        }
        Record p1 = recordList;
        while(p1) {
            if(strcmp(p1->name, _name) == 0) {
                printf("Error type 3 at Line %d: Redefined variable %s\n", _lineno, _name);
                free(newVar);
                return;
            }
            p1 = p1->next;
        }
        newVar->next = varTable[pos];
        varTable[pos] = newVar;
    }
}

int typeEqual(Type t1, Type t2) {
    if(t1 == NULL || t2 == NULL) 
        return 0;
    if(t1->kind != t2->kind) {
        return 0;
    }
    if(t1->kind == BASIC) {
        if(t1->u.basic != t2->u.basic) {
            return 0;
        }
        return 1;
    }
    else if(t1->kind == ARRAY) {
        return typeEqual(t1->u.array.elem, t2->u.array.elem);
    }
    else {
        if(t1->u.stru_index != t2->u.stru_index) {
            return 0;
        }
        return 1;
    }
    return 0;
}

void insertR() {
    Func r = (Func)malloc(sizeof(struct Func_));
    strcpy(r->name, "read");
    r->retType = (Type)malloc(sizeof(struct Type_));
    r->retType->kind = BASIC;
    r->retType->u.basic = 1;
    r->params = (FieldList)malloc(sizeof(struct FieldList_));
    r->params->name[0] = '\0';
    r->params->tail = NULL;
    r->params->type = NULL;
    int pos = hash_pjw(r->name);
    r->next = funcTable[pos];
    funcTable[pos] = r;
}

void insertW() {
    Func w = (Func)malloc(sizeof(struct Func_));
    strcpy(w->name, "write");
    w->retType = (Type)malloc(sizeof(struct Type_));
    w->retType->kind = BASIC;
    w->retType->u.basic = 1;
    w->params = (FieldList)malloc(sizeof(struct FieldList_));
    w->params->name[0] = '\0';
    w->params->tail = NULL;
    w->params->type = NULL;
    FieldList p = (FieldList)malloc(sizeof(struct FieldList_));
    p->name[0] = '\0';
    p->type = (Type)malloc(sizeof(struct Type_));
    p->type->kind = BASIC;
    p->type->u.basic = 1;
    p->tail = NULL;
    w->params->tail = p;
    int pos = hash_pjw(w->name);
    w->next = funcTable[pos];
    funcTable[pos] = w;
}