#include <stdio.h>
#include <string.h>
#include "tree.h"
#include "table.h"
#include "handler.h"
#include "translate.h"
#include "intercode.h"
#include "finalcode.h"

extern Var varTable[1024];
extern Func funcTable[1024];

extern struct TreeNode *Root;

int valid = 1;

int exist_stru = 0;

void analyze(struct TreeNode *root)
{
    if (root == NULL)
        return;
    if (strcmp(root->name, "ExtDef") == 0)
    {
        handle_extdef(root);
        return;
    }
    for (int i = 0; i < root->childNum; ++i)
    {
        analyze(root->children[i]);
    }
}

void translate(struct TreeNode *root)
{
    if (root == NULL)
        return;
    if (root->childNum == 2)
    {
        translate_extdef(root->children[0]);
        translate(root->children[1]);
    }
}

int main(int argc, char **argv)
{
    if (argc <= 1)
        return 1;
    FILE *f = fopen(argv[1], "r");
    if (!f)
    {
        perror(argv[1]);
        return 1;
    }
    yyrestart(f);
    yyparse();
    // if (valid == 1)
    //{
    //     printAll();
    // }
    memset(varTable, 0, sizeof(varTable));
    memset(funcTable, 0, sizeof(funcTable));
    insertR();
    insertW();
    analyze(Root);
    if (exist_stru == 1)
    {
        printf("Cannot translate: Code contains variables or parameters of structure type.\n");
        return 0;
    }
    translate(Root->children[0]);
    FILE *fp = fopen(argv[2], "w+");
    
    //printIntercodes(fp);
    initialWork(fp);
    fclose(fp);
    return 0;
}
