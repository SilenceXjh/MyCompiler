#include "tree.h"
#include "table.h"
#include "handler.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

extern Record recordList;
extern int record_count;
extern int varNum;
extern int exist_stru;

extern Var varTable[1024];
extern Func funcTable[1024];
extern FieldList curParams;

void handle_extdef(struct TreeNode *root)
{
    if (root->childNum == 2)
    {
        if (root->children[0]->children[0]->childNum > 1)
        {
            handle_struct(root->children[0]->children[0]);
        }
    }
    else
    {
        Type base = handle_specifier(root->children[0]);
        if (strcmp(root->children[1]->name, "ExtDecList") == 0)
        {
            handle_extdeclist(base, root->children[1]);
        }
        else
        {
            handle_funDec(base, root->children[1]);
            handle_compSt(base, root->children[2]);
        }
    }
}

void handle_extdeclist(Type base, struct TreeNode *root)
{
    handle_vardec_global(base, root->children[0]);
    if (root->childNum > 1)
    {
        handle_extdeclist(base, root->children[2]);
    }
}

void handle_vardec_global(Type cur, struct TreeNode *root)
{
    if (root->childNum == 1)
    {
        fillin_vartable_global(root->children[0]->extraInfo.word, cur, root->extraInfo.lineno);
    }
    else
    {
        Type nxt = (Type)malloc(sizeof(struct Type_));
        nxt->kind = ARRAY;
        nxt->u.array.elem = cur;
        nxt->u.array.size = root->children[2]->extraInfo.valInt;
        handle_vardec_global(nxt, root->children[0]);
    }
}

Type handle_specifier(struct TreeNode *root)
{
    Type t = (Type)malloc(sizeof(struct Type_));
    if (strcmp(root->children[0]->name, "TYPE") == 0)
    {
        if (root->children[0]->extraInfo.iof == 0)
        {
            t->kind = BASIC;
            t->u.basic = 1;
        }
        else
        {
            t->kind = BASIC;
            t->u.basic = 2;
        }
    }
    else
    {
        int id = handle_struct(root->children[0]);
        t->kind = STRUCTURE;
        t->u.stru_index = id;
    }
    t->weight = 4;
    t->isParam = 0;
    return t;
}

int handle_struct(struct TreeNode *root)
{
    if (root->childNum == 2)
    {
        int id = query_record(root->children[1]->children[0]->extraInfo.word);
        if (id == -1)
        {
            printf("Error type 17 at Line %d: Undefined structure\n",
                   root->children[1]->extraInfo.lineno);
            return -1;
        }
        else
        {
            return id;
        }
    }
    else
    {
        Record newRecord = (Record)malloc(sizeof(struct Record_));
        if (root->children[1] && root->children[1]->childNum == 1)
        {
            int id = query_record(root->children[1]->children[0]->extraInfo.word);
            if (id != -1)
            {
                printf("Error type 16 at Line %d: Duplicated name\n",
                       root->children[1]->extraInfo.lineno);
                free(newRecord);
                return -1;
            }
            strcpy(newRecord->name, root->children[1]->children[0]->extraInfo.word);
        }
        else
        {
            newRecord->name[0] = '\0';
        }
        newRecord->index = record_count;
        record_count++;
        newRecord->next = NULL;
        newRecord->fieldlist = (FieldList)malloc(sizeof(struct FieldList_));
        newRecord->fieldlist->name[0] = '\0';
        newRecord->fieldlist->type = NULL;
        newRecord->fieldlist->tail = NULL;
        handle_structDefList(newRecord->fieldlist, root->children[3]);
        if (recordList == NULL)
        {
            recordList = newRecord;
        }
        else
        {
            newRecord->next = recordList;
            recordList = newRecord;
        }
    }
}

void handle_structDefList(FieldList head, struct TreeNode *root)
{
    if (root != NULL && root->childNum == 2)
    {
        add_structDef(head, root->children[0]);
        handle_structDefList(head, root->children[1]);
    }
}

void add_structDef(FieldList head, struct TreeNode *root)
{
    Type t = handle_specifier(root->children[0]);
    add_structDecList(head, t, root->children[1]);
}

void add_structDecList(FieldList head, Type _t, struct TreeNode *root)
{
    add_structDec(head, _t, root->children[0]);
    if (root->childNum > 1)
    {
        add_structDecList(head, _t, root->children[2]);
    }
}

void add_structDec(FieldList head, Type _t, struct TreeNode *root)
{
    if (root->childNum > 1)
    {
        printf("Error type 15 at Line %d: initialize when define structure\n",
               root->children[0]->extraInfo.lineno);
        return;
    }
    add_structVarDec(head, _t, root->children[0]);
}

void add_structVarDec(FieldList head, Type _t, struct TreeNode *root)
{
    if (root->childNum == 1)
    {
        FieldList newField = (FieldList)malloc(sizeof(struct FieldList_));
        newField->type = _t;
        strcpy(newField->name, root->children[0]->extraInfo.word);
        newField->tail = NULL;

        FieldList p = head;
        while (p->tail)
        {
            if (strcmp(p->name, newField->name) == 0)
            {
                free(newField);
                printf("Error type 15 at Line %d: Redefined field %s\n",
                       root->extraInfo.lineno, p->name);
                return;
            }
            p = p->tail;
        }
        if (strcmp(p->name, newField->name) == 0)
        {
            free(newField);
            printf("Error type 15 at Line %d: Redefined field %s\n",
                   root->extraInfo.lineno, p->name);
            return;
        }
        p->tail = newField;
    }
    else
    {
        Type nxt = (Type)malloc(sizeof(struct Type_));
        nxt->kind = ARRAY;
        nxt->u.array.elem = _t;
        nxt->u.array.size = root->children[2]->extraInfo.valInt;
        add_structVarDec(head, nxt, root->children[0]);
    }
}

void handle_funDec(Type t, struct TreeNode *root)
{
    Func newFunc = (Func)malloc(sizeof(struct Func_));
    newFunc->retType = t;
    strcpy(newFunc->name, root->children[0]->extraInfo.word);
    newFunc->next = NULL;
    newFunc->params = (FieldList)malloc(sizeof(struct FieldList_));
    newFunc->params->name[0] = '\0';
    newFunc->params->tail = NULL;
    newFunc->params->type = NULL;
    int pos = hash_pjw(newFunc->name);
    Func p = funcTable[pos];
    while (p)
    {
        if (strcmp(p->name, newFunc->name) == 0)
        {
            printf("Error type 4 at Line %d: Redefined function %s\n",
                   root->extraInfo.lineno, p->name);
            return;
        }
        p = p->next;
    }
    if (root->childNum == 4)
    {
        handle_valList(newFunc->params, root->children[2]);
    }
    newFunc->next = funcTable[pos];
    funcTable[pos] = newFunc;
    curParams = newFunc->params;
}

void handle_valList(FieldList params, struct TreeNode *root)
{
    add_paramDec(params, root->children[0]);
    if (root->childNum > 1)
    {
        handle_valList(params, root->children[2]);
    }
}

void add_paramDec(FieldList params, struct TreeNode *root)
{
    Type t = handle_specifier(root->children[0]);
    t->isParam = 1;
    if (t->kind == STRUCTURE)
    {
        exist_stru = 1;
    }
    add_paramVarDec(params, t, root->children[1]);
}

void add_paramVarDec(FieldList head, Type _t, struct TreeNode *root)
{
    if (root->childNum == 1)
    {
        FieldList newField = (FieldList)malloc(sizeof(struct FieldList_));
        newField->type = _t;
        strcpy(newField->name, root->children[0]->extraInfo.word);
        newField->tail = NULL;

        FieldList p = head;
        while (p->tail)
        {
            if (strcmp(p->name, newField->name) == 0)
            {
                free(newField);
                printf("Error type 3 at Line %d: Redefined param %s\n",
                       root->extraInfo.lineno, p->name);
                return;
            }
            p = p->tail;
        }
        if (strcmp(p->name, newField->name) == 0)
        {
            free(newField);
            printf("Error type 3 at Line %d: Redefined param %s\n",
                   root->extraInfo.lineno, p->name);
            return;
        }
        p->tail = newField;

        Var newVar = (Var)malloc(sizeof(struct Var_));
        newVar->t = _t;
        strcpy(newVar->name, newField->name);
        newVar->index = varNum;
        varNum++;
        int pos = hash_pjw(newVar->name);
        newVar->next = varTable[pos];
        varTable[pos] = newVar;
    }
    else
    {
        Type nxt = (Type)malloc(sizeof(struct Type_));
        nxt->kind = ARRAY;
        nxt->u.array.elem = _t;
        nxt->u.array.size = root->children[2]->extraInfo.valInt;
        if(_t->kind == BASIC) {
            nxt->weight = 4;
        }
        else {
            nxt->weight = _t->u.array.size * _t->weight;
        }
        nxt->isParam = 1;
        add_paramVarDec(head, nxt, root->children[0]);
    }
}

void handle_compSt(Type ret, struct TreeNode *root)
{
    handle_defList(root->children[1]);
    handle_stmtList(ret, root->children[2]);
}

void handle_defList(struct TreeNode *root)
{
    if (root == NULL)
        return;
    if (root->childNum == 2)
    {
        handle_def(root->children[0]);
        handle_defList(root->children[1]);
    }
}

void handle_def(struct TreeNode *root)
{
    Type _t = handle_specifier(root->children[0]);
    if (_t->kind == STRUCTURE)
    {
        exist_stru = 1;
    }
    handle_decList(_t, root->children[1]);
}

void handle_decList(Type _t, struct TreeNode *root)
{
    handle_dec(_t, root->children[0]);
    if (root->childNum == 3)
    {
        handle_decList(_t, root->children[2]);
    }
}

void handle_dec(Type _t, struct TreeNode *root)
{
    handle_varDec(_t, root->children[0]);
    if (root->childNum == 3)
    {
        Type back = handle_exp(root->children[2]);
        if (!typeEqual(_t, back))
        {
            printf("Error type 5 at Line %d: Type mismatched for assignment\n",
                   root->extraInfo.lineno);
        }
    }
}

void handle_varDec(Type _t, struct TreeNode *root)
{
    if (root->childNum == 1)
    {
        FieldList p = curParams;
        while (p)
        {
            if (strcmp(p->name, root->children[0]->extraInfo.word) == 0)
            {
                printf("Error type 3 at Line %d: Redefined variable %s\n",
                       root->extraInfo.lineno, p->name);
                return;
            }
            p = p->tail;
        }
        Var newVar = (Var)malloc(sizeof(struct Var_));
        newVar->t = _t;
        newVar->index = varNum;
        varNum++;
        strcpy(newVar->name, root->children[0]->extraInfo.word);
        int pos = hash_pjw(newVar->name);
        newVar->next = varTable[pos];
        varTable[pos] = newVar;
    }
    else
    {
        Type nxt = (Type)malloc(sizeof(struct Type_));
        nxt->kind = ARRAY;
        nxt->u.array.elem = _t;
        nxt->u.array.size = root->children[2]->extraInfo.valInt;
        if (_t->kind == BASIC || _t->kind == STRUCTURE)
        {
            nxt->weight = 4;
        }
        else
        {
            nxt->weight = _t->u.array.size * _t->weight;
        }
        nxt->isParam = 0;
        handle_varDec(nxt, root->children[0]);
    }
}

Type handle_exp(struct TreeNode *root)
{
    Type ret = NULL;
    if (root->childNum == 1)
    {
        if (strcmp(root->children[0]->name, "ID") == 0)
        {
            ret = getVarType(root->children[0]->extraInfo.word);
            if (ret == NULL)
            {
                printf("Error type 1 at Line %d: Undefined variable %s\n",
                       root->extraInfo.lineno, root->children[0]->extraInfo.word);
            }
        }
        else if (strcmp(root->children[0]->name, "INT") == 0)
        {
            ret = (Type)malloc(sizeof(struct Type_));
            ret->kind = BASIC;
            ret->u.basic = 1;
            return ret;
        }
        else
        {
            ret = (Type)malloc(sizeof(struct Type_));
            ret->kind = BASIC;
            ret->u.basic = 2;
            return ret;
        }
    }
    else if (root->childNum == 2)
    {
        ret = handle_exp(root->children[1]);
        if (ret == NULL || ret->kind != BASIC)
        {
            printf("Error type 7 at Line %d: Type mismatched for operands\n",
                   root->children[1]->extraInfo.lineno);
        }
        else if (strcmp(root->children[0]->name, "NOT") == 0)
        {
            if (ret->u.basic != 1)
            {
                printf("Error type 7 at Line %d: Type mismatched for operands\n",
                       root->children[1]->extraInfo.lineno);
            }
        }
    }
    else if (root->childNum == 3)
    {
        if (strcmp(root->children[1]->name, "ASSIGNOP") == 0)
        {
            judgeLeftValue(root->children[0]);
            Type left = handle_exp(root->children[0]);
            Type right = handle_exp(root->children[2]);
            if (left == NULL || right == NULL)
            {
                return NULL;
            }
            if (!typeEqual(left, right))
            {
                printf("Error type 5 at Line %d: Type mismatched for assignment\n",
                       root->extraInfo.lineno);
            }
            ret = left;
        }
        else if (strcmp(root->children[1]->name, "AND") == 0 ||
                 strcmp(root->children[1]->name, "OR") == 0)
        {
            Type t1 = handle_exp(root->children[0]);
            Type t2 = handle_exp(root->children[2]);
            if (t1 == NULL || t2 == NULL)
            {
                return NULL;
            }
            if (t1->kind != BASIC || t1->u.basic != 1 || t2->kind != BASIC || t2->u.basic != 1)
            {
                printf("Error type 7 at Line %d: Type mismatched for operands\n",
                       root->extraInfo.lineno);
            }
            ret = t1;
        }
        else if (strcmp(root->children[1]->name, "RELOP") == 0 ||
                 strcmp(root->children[1]->name, "PLUS") == 0 ||
                 strcmp(root->children[1]->name, "MINUS") == 0 ||
                 strcmp(root->children[1]->name, "STAR") == 0 ||
                 strcmp(root->children[1]->name, "DIV") == 0)
        {
            Type t1 = handle_exp(root->children[0]);
            Type t2 = handle_exp(root->children[2]);
            if (t1 == NULL || t2 == NULL)
            {
                return NULL;
            }
            if (t1->kind != BASIC || t2->kind != BASIC || t1->u.basic != t2->u.basic)
            {
                printf("Error type 7 at Line %d: Type mismatched for operands\n",
                       root->extraInfo.lineno);
                return NULL;
            }
            if (strcmp(root->children[1]->name, "RELOP") == 0)
            {
                ret = (Type)malloc(sizeof(struct Type_));
                ret->kind = BASIC;
                ret->u.basic = 1;
            }
            else
            {
                ret = t1;
            }
        }
        else if (strcmp(root->children[1]->name, "Exp") == 0)
        {
            ret = handle_exp(root->children[1]);
        }
        else if (strcmp(root->children[0]->name, "ID") == 0)
        {
            Type tmp = getVarType(root->children[0]->extraInfo.word);
            if (tmp != NULL)
            {
                printf("Error type 11 at Line %d: '%s' is not a function\n",
                       root->extraInfo.lineno, root->children[0]->extraInfo.word);
                return NULL;
            }
            FieldList params = getFuncParams(root->children[0]->extraInfo.word);
            if (params == NULL)
            {
                printf("Error type 2 at Line %d: Undefined function %s\n",
                       root->extraInfo.lineno, root->children[0]->extraInfo.word);
                return NULL;
            }
            else if (params->tail != NULL)
            {
                printf("Error type 9 at Line %d: arguments unfit\n",
                       root->extraInfo.lineno);
            }
            ret = getFuncRet(root->children[0]->extraInfo.word);
        }
        else if (strcmp(root->children[1]->name, "DOT") == 0)
        {
            Type front = handle_exp(root->children[0]);
            if (front == NULL)
            {
                return NULL;
            }
            if (front->kind != STRUCTURE)
            {
                printf("Error type 13 at Line %d: Illegal use of '.'\n",
                       root->extraInfo.lineno);
                return NULL;
            }
            Record p = recordList;
            while (p)
            {
                if (p->index == front->u.stru_index)
                {
                    break;
                }
                p = p->next;
            }
            if (p == NULL)
            {
                return NULL;
            }
            FieldList p1 = p->fieldlist;
            while (p1)
            {
                if (strcmp(p1->name, root->children[2]->extraInfo.word) == 0)
                {
                    break;
                }
                p1 = p1->tail;
            }
            if (p1 == NULL)
            {
                printf("Error type 14 at Line %d: Non-existent field 'root->children[2]->extraInfo.word'\n",
                       root->extraInfo.lineno);
                return NULL;
            }
            ret = p1->type;
        }
    }
    else
    {
        if (strcmp(root->children[0]->name, "ID") == 0)
        {
            Type tmp = getVarType(root->children[0]->extraInfo.word);
            if (tmp != NULL)
            {
                printf("Error type 11 at Line %d: '%s' is not a function\n",
                       root->extraInfo.lineno, root->children[0]->extraInfo.word);
                return NULL;
            }
            FieldList paramList = getFuncParams(root->children[0]->extraInfo.word);
            if (paramList == NULL)
            {
                printf("Error type 2 at Line %d: Undefined function %s\n",
                       root->extraInfo.lineno, root->children[0]->extraInfo.word);
                return NULL;
            }
            judge_args(paramList->tail, root->children[2]);
            ret = getFuncRet(root->children[0]->extraInfo.word);
        }
        else
        {
            Type head = handle_exp(root->children[0]);
            Type body = handle_exp(root->children[2]);
            if (head == NULL || head->kind != ARRAY)
            {
                printf("Error type 10 at Line %d: not array\n",
                       root->children[0]->extraInfo.lineno);
                // return NULL;
            }
            if (body == NULL || body->kind != BASIC || body->u.basic != 1)
            {
                printf("Error type 12 at Line %d: non-integer in array\n",
                       root->children[0]->extraInfo.lineno);
                return NULL;
            }
            if (head != NULL && head->kind == ARRAY)
            {
                ret = head->u.array.elem;
            }
        }
    }
    return ret;
}

void judge_args(FieldList params, struct TreeNode *root)
{
    if (params == NULL)
    {
        printf("Error type 9 at Line %d: arguments unfit\n",
               root->extraInfo.lineno);
        return;
    }
    Type cur = handle_exp(root->children[0]);
    if (!typeEqual(params->type, cur))
    {
        printf("Error type 9 at Line %d: arguments unfit\n",
               root->extraInfo.lineno);
        return;
    }
    if (root->childNum == 1 && params->tail != NULL)
    {
        printf("Error type 9 at Line %d: arguments unfit\n",
               root->extraInfo.lineno);
        return;
    }
    if (root->childNum == 1)
    {
        return;
    }
    judge_args(params->tail, root->children[2]);
}

void judgeLeftValue(struct TreeNode *root)
{
    if (root->childNum == 1 && strcmp(root->children[0]->name, "ID") == 0)
        return;
    if (root->childNum == 3 && strcmp(root->children[1]->name, "DOT") == 0)
        return;
    if (root->childNum == 4 && strcmp(root->children[0]->name, "Exp") == 0)
        return;
    printf("Error type 6 at Line %d: The left-hand value error\n",
           root->extraInfo.lineno);
}

void handle_stmtList(Type ret, struct TreeNode *root)
{
    if (root == NULL)
        return;
    if (root->childNum == 2)
    {
        handle_stmt(ret, root->children[0]);
        handle_stmtList(ret, root->children[1]);
    }
}

void handle_stmt(Type ret, struct TreeNode *root)
{
    if (root->childNum == 1)
    {
        handle_compSt(ret, root->children[0]);
    }
    else if (root->childNum == 2)
    {
        handle_exp(root->children[0]);
    }
    else if (root->childNum == 3)
    {
        Type _ret = handle_exp(root->children[1]);
        if (!typeEqual(_ret, ret))
        {
            printf("Error type 8 at Line %d: Type mismatched for return\n",
                   root->extraInfo.lineno);
        }
    }
    else if (root->childNum == 5)
    {
        Type cond = handle_exp(root->children[2]);
        if (cond == NULL)
            return;
        if (cond->kind != BASIC || cond->u.basic != 1)
        {
            printf("Error type 7 at Line %d: condition must be integer\n",
                   root->children[2]->extraInfo.lineno);
        }
        handle_stmt(ret, root->children[4]);
    }
    else
    {
        Type cond = handle_exp(root->children[2]);
        if (cond == NULL)
            return;
        if (cond->kind != BASIC || cond->u.basic != 1)
        {
            printf("Error type 7 at Line %d: condition must be integer\n",
                   root->children[2]->extraInfo.lineno);
        }
        handle_stmt(ret, root->children[4]);
        handle_stmt(ret, root->children[6]);
    }
}