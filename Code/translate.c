#include <stdio.h>
#include <string.h>
#include "translate.h"

void translate_extdef(struct TreeNode *root)
{
    if (strcmp(root->children[1]->name, "FunDec") == 0)
    {
        Intercodes code1 = translate_fundec(root->children[1]);
        Intercodes code2 = translate_compSt(root->children[2]);
        insertCode(code1);
        insertCode(code2);
    }
}

Intercodes translate_fundec(struct TreeNode *root)
{
    Intercodes code1 = genFunDec(root->children[0]->extraInfo.word);
    if (root->childNum == 4)
    {
        Intercodes code2 = translate_varlist(root->children[2]);
        code1 = mergeCode(code1, code2);
    }
    return code1;
}

Intercodes translate_varlist(struct TreeNode *root)
{
    Intercodes code1 = translate_param(root->children[0]);
    if (root->childNum > 2)
    {
        Intercodes code2 = translate_varlist(root->children[2]);
        code1 = mergeCode(code1, code2);
    }
    return code1;
}

Intercodes translate_param(struct TreeNode *root)
{
    return translate_paramVardec(root->children[1]);
}

Intercodes translate_paramVardec(struct TreeNode *root)
{
    if (root->childNum == 1)
    {
        Type t = getVarType(root->children[0]->extraInfo.word);
        Intercodes code1 = genParam(root->children[0]->extraInfo.word);
        return code1;
    }
    return translate_paramVardec(root->children[0]);
}

Intercodes translate_compSt(struct TreeNode *root)
{
    Intercodes code1 = translate_deflist(root->children[1]);
    Intercodes code2 = translate_stmtlist(root->children[2]);
    code1 = mergeCode(code1, code2);
    return code1;
}

Intercodes translate_deflist(struct TreeNode *root)
{
    if (root == NULL)
        return NULL;
    if (root->childNum == 2)
    {
        Intercodes code1 = translate_def(root->children[0]);
        Intercodes code2 = translate_deflist(root->children[1]);
        code1 = mergeCode(code1, code2);
        return code1;
    }
    return NULL;
}

Intercodes translate_def(struct TreeNode *root)
{
    return translate_declist(root->children[1]);
}

Intercodes translate_declist(struct TreeNode *root)
{
    Intercodes code1 = translate_dec(root->children[0]);
    if (root->childNum > 2)
    {
        Intercodes code2 = translate_declist(root->children[2]);
        code1 = mergeCode(code1, code2);
    }
    return code1;
}

Intercodes translate_dec(struct TreeNode *root)
{
    if (root->childNum == 1)
    {
        return translate_vardec(root->children[0]);
    }
    if (root->children[0]->childNum > 1)
    {
        return NULL;
    }
    int no = getVarIndex(root->children[0]->children[0]->extraInfo.word);
    Operand v1 = genVariable(no);
    Intercodes code1 = translate_exp(root->children[2], v1);
    return code1;
}

Intercodes translate_vardec(struct TreeNode *root)
{
    if (root->childNum == 1)
    {
        Type t = getVarType(root->children[0]->extraInfo.word);
        if (t->kind == BASIC)
        {
            return NULL;
        }
        int size = getTypeSize(t);
        return genArrayDec(root->children[0]->extraInfo.word, size);
    }
    return translate_vardec(root->children[0]);
}

Intercodes translate_exp(struct TreeNode *root, Operand place)
{
    if (root->childNum == 1)
    {
        if (strcmp(root->children[0]->name, "ID") == 0)
        {
            int no = getVarIndex(root->children[0]->extraInfo.word);
            Type t = getVarType(root->children[0]->extraInfo.word);
            if (t->kind == BASIC || t->isParam == 1)
            {
                Operand right = genVariable(no);
                return genAssign(place, right);
            }
            else if (t->kind == ARRAY)
            {
                Operand right = genAddr(no);
                return genAssign(place, right);
            }
        }
        else if (strcmp(root->children[0]->name, "INT") == 0)
        {
            int value = root->children[0]->extraInfo.valInt;
            Operand right = genInt(value);
            return genAssign(place, right);
        }
    }
    else if (root->childNum == 2)
    {
        if (strcmp(root->children[0]->name, "MINUS") == 0)
        {
            Operand t1 = genTemp();
            Intercodes code1 = translate_exp(root->children[1], t1);
            Operand zero = genInt(0);
            Intercodes code2 = genSub(place, zero, t1);
            code1 = mergeCode(code1, code2);
            return code1;
        }
        else if (strcmp(root->children[0]->name, "NOT") == 0)
        {
            return cond_general(root, place);
        }
    }
    else if (root->childNum == 3)
    {
        if (strcmp(root->children[1]->name, "ASSIGNOP") == 0)
        {
            Operand t1 = genTemp();
            Intercodes code1 = translate_exp(root->children[2], t1);
            if (root->children[0]->childNum == 1)
            {
                int no = getVarIndex(root->children[0]->children[0]->extraInfo.word);
                Operand v1 = genVariable(no);
                Intercodes code2 = genAssign(v1, t1);
                Intercodes code3 = genAssign(place, v1);
                code2 = mergeCode(code2, code3);
                return mergeCode(code1, code2);
            }
            else
            {
                Operand t2 = genTemp();
                Intercodes code2 = cal_arrayAddr(root->children[0], t2);
                Intercodes code3 = genLeftStar(t2, t1);
                code2 = mergeCode(code2, code3);
                return mergeCode(code1, code2);
            }
        }
        else if (strcmp(root->children[1]->name, "AND") == 0 ||
                 strcmp(root->children[1]->name, "OR") == 0 ||
                 strcmp(root->children[1]->name, "RELOP") == 0)
        {
            return cond_general(root, place);
        }
        else if (strcmp(root->children[1]->name, "PLUS") == 0)
        {
            Operand t1 = genTemp();
            Operand t2 = genTemp();
            Intercodes code1 = translate_exp(root->children[0], t1);
            Intercodes code2 = translate_exp(root->children[2], t2);
            Intercodes code3 = genAdd(place, t1, t2);
            code2 = mergeCode(code2, code3);
            return mergeCode(code1, code2);
        }
        else if (strcmp(root->children[1]->name, "MINUS") == 0)
        {
            Operand t1 = genTemp();
            Operand t2 = genTemp();
            Intercodes code1 = translate_exp(root->children[0], t1);
            Intercodes code2 = translate_exp(root->children[2], t2);
            Intercodes code3 = genSub(place, t1, t2);
            code2 = mergeCode(code2, code3);
            return mergeCode(code1, code2);
        }
        else if (strcmp(root->children[1]->name, "STAR") == 0)
        {
            Operand t1 = genTemp();
            Operand t2 = genTemp();
            Intercodes code1 = translate_exp(root->children[0], t1);
            Intercodes code2 = translate_exp(root->children[2], t2);
            Intercodes code3 = genMul(place, t1, t2);
            code2 = mergeCode(code2, code3);
            return mergeCode(code1, code2);
        }
        else if (strcmp(root->children[1]->name, "DIV") == 0)
        {
            Operand t1 = genTemp();
            Operand t2 = genTemp();
            Intercodes code1 = translate_exp(root->children[0], t1);
            Intercodes code2 = translate_exp(root->children[2], t2);
            Intercodes code3 = genDiv(place, t1, t2);
            code2 = mergeCode(code2, code3);
            return mergeCode(code1, code2);
        }
        else if (strcmp(root->children[1]->name, "Exp") == 0)
        {
            return translate_exp(root->children[1], place);
        }
        else if (strcmp(root->children[0]->name, "ID") == 0)
        {
            if (strcmp(root->children[0]->extraInfo.word, "read") == 0)
            {
                return genRead(place);
            }
            else
            {
                return genCall(place, root->children[0]->extraInfo.word);
            }
        }
    }
    else if (root->childNum == 4)
    {
        if (strcmp(root->children[0]->name, "ID") == 0)
        {
            Arg arg_list = genArg(NULL);
            Intercodes code1 = translate_args(root->children[2], arg_list);
            if (strcmp(root->children[0]->extraInfo.word, "write") == 0)
            {
                Intercodes code2 = genWrite(arg_list->next->op);
                Operand zero = genInt(0);
                Intercodes code3 = genAssign(place, zero);
                code2 = mergeCode(code2, code3);
                return mergeCode(code1, code2);
            }
            else
            {
                Intercodes code2 = genArgDec(arg_list->next);
                Intercodes code3 = genCall(place, root->children[0]->extraInfo.word);
                code2 = mergeCode(code2, code3);
                return mergeCode(code1, code2);
            }
        }
        else
        {
            Operand t1 = genTemp();
            Intercodes code1 = cal_arrayAddr(root, t1);
            Intercodes code2 = genRightStar(place, t1);
            return mergeCode(code1, code2);
        }
    }
    return NULL;
}

Intercodes cal_arrayAddr(struct TreeNode *root, Operand place)
{
    if (root->childNum == 1)
    {
        int no = getVarIndex(root->children[0]->extraInfo.word);
        Type t = getVarType(root->children[0]->extraInfo.word);
        Operand v = NULL;
        if (t->isParam == 1)
        {
            v = genVariable(no);
        }
        else
        {
            v = genAddr(no);
        }
        Intercodes code1 = genAssign(place, v);
        return code1;
    }
    else if (root->childNum == 4)
    {
        Operand t1 = genTemp();
        Operand t2 = genTemp();
        Intercodes code1 = cal_arrayAddr(root->children[0], t1);
        Intercodes code2 = translate_exp(root->children[2], t2);
        int curWright = getExpType(root->children[0])->weight;
        Operand w = genInt(curWright);
        Operand t3 = genTemp();
        Intercodes code3 = genMul(t3, t2, w);
        Intercodes code4 = genAdd(place, t1, t3);
        code3 = mergeCode(code3, code4);
        code2 = mergeCode(code2, code3);
        return mergeCode(code1, code2);
    }
    return NULL;
}

Type getExpType(struct TreeNode *root)
{
    Type t = NULL;
    if (root->childNum == 1)
    {
        t = getVarType(root->children[0]->extraInfo.word);
    }
    else
    {
        t = getExpType(root->children[0])->u.array.elem;
    }
    return t;
}

Intercodes translate_args(struct TreeNode *root, Arg arg_list)
{
    Operand t1 = genTemp();
    Intercodes code1 = translate_exp(root->children[0], t1);
    Arg p = arg_list;
    while (p->next)
    {
        p = p->next;
    }
    Arg arg = genArg(t1);
    p->next = arg;
    if (root->childNum > 2)
    {
        Intercodes code2 = translate_args(root->children[2], arg_list);
        code1 = mergeCode(code1, code2);
    }
    return code1;
}

Intercodes cond_general(struct TreeNode *root, Operand place)
{
    Operand label1 = genLabel();
    Operand label2 = genLabel();
    Operand zero = genInt(0);
    Intercodes code0 = genAssign(place, zero);
    Intercodes code1 = translate_cond(root, label1, label2);
    Intercodes code2 = genLabelDec(label1);
    Operand one = genInt(1);
    Intercodes code3 = genAssign(place, one);
    code2 = mergeCode(code2, code3);
    Intercodes code4 = genLabelDec(label2);
    code0 = mergeCode(code0, code1);
    code0 = mergeCode(code0, code2);
    code0 = mergeCode(code0, code4);
    return code0;
}

Intercodes translate_cond(struct TreeNode *root, Operand label_true, Operand label_false)
{
    if (root->childNum == 2)
    {
        if (strcmp(root->children[0]->name, "NOT") == 0)
        {
            return translate_cond(root->children[1], label_false, label_true);
        }
    }
    else if (root->childNum == 3)
    {
        if (strcmp(root->children[1]->name, "RELOP") == 0)
        {
            Operand t1 = genTemp();
            Operand t2 = genTemp();
            Intercodes code1 = translate_exp(root->children[0], t1);
            Intercodes code2 = translate_exp(root->children[2], t2);
            char rel[3];
            strcpy(rel, root->children[1]->extraInfo.relop);
            Intercodes code3 = genIfGoto(t1, t2, rel, label_true);
            Intercodes code4 = genGoto(label_false);
            code3 = mergeCode(code3, code4);
            code1 = mergeCode(code1, code2);
            code1 = mergeCode(code1, code3);
            return code1;
        }
        else if (strcmp(root->children[1]->name, "AND") == 0)
        {
            Operand label1 = genLabel();
            Intercodes code1 = translate_cond(root->children[0], label1, label_false);
            Intercodes code2 = translate_cond(root->children[2], label_true, label_false);
            Intercodes mid = genLabelDec(label1);
            code1 = mergeCode(code1, mid);
            code1 = mergeCode(code1, code2);
            return code1;
        }
        else if (strcmp(root->children[1]->name, "OR") == 0)
        {
            Operand label1 = genLabel();
            Intercodes code1 = translate_cond(root->children[0], label_true, label1);
            Intercodes code2 = translate_cond(root->children[2], label_true, label_false);
            Intercodes mid = genLabelDec(label1);
            code1 = mergeCode(code1, mid);
            code1 = mergeCode(code1, code2);
            return code1;
        }
    }
    Operand t1 = genTemp();
    Intercodes code1 = translate_exp(root, t1);
    char rel[3];
    strcpy(rel, "!=");
    Operand zero = genInt(0);
    Intercodes code2 = genIfGoto(t1, zero, rel, label_true);
    Intercodes code3 = genGoto(label_false);
    code2 = mergeCode(code2, code3);
    code1 = mergeCode(code1, code2);
    return code1;
}

Intercodes translate_stmtlist(struct TreeNode *root)
{
    if (root == NULL)
        return NULL;
    if (root->childNum == 2)
    {
        Intercodes code1 = translate_stmt(root->children[0]);
        Intercodes code2 = translate_stmtlist(root->children[1]);
        code1 = mergeCode(code1, code2);
        return code1;
    }
    return NULL;
}

Intercodes translate_stmt(struct TreeNode *root)
{
    if (root->childNum == 1)
    {
        return translate_compSt(root->children[0]);
    }
    else if (root->childNum == 2)
    {
        return translate_exp(root->children[0], NULL);
    }
    else if (root->childNum == 3)
    {
        Operand t1 = genTemp();
        Intercodes code1 = translate_exp(root->children[1], t1);
        Intercodes code2 = genReturn(t1);
        code1 = mergeCode(code1, code2);
        return code1;
    }
    else if (root->childNum == 5)
    {
        if (strcmp(root->children[0]->name, "IF") == 0)
        {
            Operand label1 = genLabel();
            Operand label2 = genLabel();
            Intercodes code1 = translate_cond(root->children[2], label1, label2);
            Intercodes code2 = translate_stmt(root->children[4]);
            Intercodes mid = genLabelDec(label1);
            Intercodes end = genLabelDec(label2);
            code1 = mergeCode(code1, mid);
            code2 = mergeCode(code2, end);
            code1 = mergeCode(code1, code2);
            return code1;
        }
        else
        {
            Operand label1 = genLabel();
            Operand label2 = genLabel();
            Operand label3 = genLabel();
            Intercodes code1 = translate_cond(root->children[2], label2, label3);
            Intercodes code2 = translate_stmt(root->children[4]);
            Intercodes begin = genLabelDec(label1);
            code1 = mergeCode(code1, genLabelDec(label2));
            code2 = mergeCode(code2, genGoto(label1));
            code2 = mergeCode(code2, genLabelDec(label3));
            code1 = mergeCode(code1, code2);
            return mergeCode(begin, code1);
        }
    }
    else if (root->childNum == 7)
    {
        Operand label1 = genLabel();
        Operand label2 = genLabel();
        Operand label3 = genLabel();
        Intercodes code1 = translate_cond(root->children[2], label1, label2);
        Intercodes code2 = translate_stmt(root->children[4]);
        Intercodes code3 = translate_stmt(root->children[6]);
        code1 = mergeCode(code1, genLabelDec(label1));
        code2 = mergeCode(code2, genGoto(label3));
        code2 = mergeCode(code2, genLabelDec(label2));
        code3 = mergeCode(code3, genLabelDec(label3));
        code2 = mergeCode(code2, code3);
        return mergeCode(code1, code2);
    }
    return NULL;
}