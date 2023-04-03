#include "tree.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct TreeNode* Root = NULL;

struct TreeNode* createLeaf(char* _name) {
    struct TreeNode* node = (struct TreeNode*)malloc(sizeof(struct TreeNode));
    strcpy(node->name, _name);
    node->childNum = 0;
    node->type = -1;
    return node;
}

struct TreeNode* create(char* _name, int _lineno) {
    struct TreeNode* node = (struct TreeNode*)malloc(sizeof(struct TreeNode));
    strcpy(node->name, _name);
    node->childNum = 0;
    node->type = 0;
    node->extraInfo.lineno = _lineno;
    return node;
}

void setRoot(struct TreeNode* root) {
    Root = root;
}

void print(struct TreeNode* root) {
    if(root->type == 0) {   //grammer_unit
        if(root->childNum != 0) {
            printf("%s (%d)\n", root->name, root->extraInfo.lineno);
        }
    }
    else if(root->type == 1) {   //ID
        printf("%s: %s\n", root->name, root->extraInfo.word);
    }
    else if(root->type == 2) {   //TYPE
        if(root->extraInfo.iof == 0) 
            printf("TYPE: int\n");
        else 
            printf("TYPE: float\n");
    }
    else if(root->type == 3) {   //INT
        printf("%s: %d\n", root->name, root->extraInfo.valInt);
    }
    else if(root->type == 4) {   //FLOAT
        printf("%s: %f\n", root->name, root->extraInfo.valFloat);
    }
    else {
        printf("%s\n", root->name);
    }
}

void printTree(struct TreeNode* root, int space) {
    for(int i = 0; i < space; ++i) 
        printf(" ");
    print(root);
    for(int i = 0; i < root->childNum; ++i) {
        if(root->children[i]) {
            printTree(root->children[i], space+2);
        }
    }
}

void printAll() {
    printTree(Root, 0);
}

void build1(struct TreeNode* root, struct TreeNode* sub1) {
    root->childNum = 1;
    root->children = (struct TreeNode**)malloc(sizeof(struct TreeNode*) * 1);
    root->children[0] = sub1;
}

void build2(struct TreeNode* root, struct TreeNode* sub1, struct TreeNode* sub2) {
    root->childNum = 2;
    root->children = (struct TreeNode**)malloc(sizeof(struct TreeNode*) * 2);
    root->children[0] = sub1;
    root->children[1] = sub2;
}

void build3(struct TreeNode* root, struct TreeNode* sub1, struct TreeNode* sub2, 
struct TreeNode* sub3) {
    root->childNum = 3;
    root->children = (struct TreeNode**)malloc(sizeof(struct TreeNode*) * 3);
    root->children[0] = sub1;
    root->children[1] = sub2;
    root->children[2] = sub3;
}

void build4(struct TreeNode* root, struct TreeNode* sub1, struct TreeNode* sub2, 
struct TreeNode* sub3, struct TreeNode* sub4) {
    root->childNum = 4;
    root->children = (struct TreeNode**)malloc(sizeof(struct TreeNode*) * 4);
    root->children[0] = sub1;
    root->children[1] = sub2;
    root->children[2] = sub3;
    root->children[3] = sub4;
}

void build5(struct TreeNode* root, struct TreeNode* sub1, struct TreeNode* sub2, 
struct TreeNode* sub3, struct TreeNode* sub4, struct TreeNode* sub5) {
    root->childNum = 5;
    root->children = (struct TreeNode**)malloc(sizeof(struct TreeNode*) * 5);
    root->children[0] = sub1;
    root->children[1] = sub2;
    root->children[2] = sub3;
    root->children[3] = sub4;
    root->children[4] = sub5;
}

void build7(struct TreeNode* root, struct TreeNode* sub1, struct TreeNode* sub2, 
struct TreeNode* sub3, struct TreeNode* sub4, struct TreeNode* sub5, 
struct TreeNode* sub6, struct TreeNode* sub7) {
    root->childNum = 7;
    root->children = (struct TreeNode**)malloc(sizeof(struct TreeNode*) * 7);
    root->children[0] = sub1;
    root->children[1] = sub2;
    root->children[2] = sub3;
    root->children[3] = sub4;
    root->children[4] = sub5;
    root->children[5] = sub6;
    root->children[6] = sub7;
}