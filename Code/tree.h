#ifndef _TREE_H_
#define _TREE_H_

struct TreeNode{
    int type;
    int childNum;
    char name[18];
    union {
        int lineno;
        char word[34];
        int iof;
        int valInt;
        float valFloat;
        char relop[3];
    }extraInfo;
    struct TreeNode** children;
};

struct TreeNode* createLeaf(char* _name);

struct TreeNode* create(char* _name, int _lineno);

void setRoot(struct TreeNode* root);

void print(struct TreeNode* root);

void printTree(struct TreeNode* root, int space);

void printAll();

void build1(struct TreeNode* root, struct TreeNode* sub1);

void build2(struct TreeNode* root, struct TreeNode* sub1, struct TreeNode* sub2);

void build3(struct TreeNode* root, struct TreeNode* sub1, struct TreeNode* sub2, 
struct TreeNode* sub3);

void build4(struct TreeNode* root, struct TreeNode* sub1, struct TreeNode* sub2, 
struct TreeNode* sub3, struct TreeNode* sub4);

void build5(struct TreeNode* root, struct TreeNode* sub1, struct TreeNode* sub2, 
struct TreeNode* sub3, struct TreeNode* sub4, struct TreeNode* sub5);

void build7(struct TreeNode* root, struct TreeNode* sub1, struct TreeNode* sub2, 
struct TreeNode* sub3, struct TreeNode* sub4, struct TreeNode* sub5, 
struct TreeNode* sub6, struct TreeNode* sub7);

#endif