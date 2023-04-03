#include "tree.h"
#include "table.h"
#include "intercode.h"

void translate_extdef(struct TreeNode* root);

Intercodes translate_fundec(struct TreeNode* root);

Intercodes translate_varlist(struct TreeNode* root);

Intercodes translate_param(struct TreeNode* root);

Intercodes translate_paramVardec(struct TreeNode* root);

Intercodes translate_compSt(struct TreeNode* root);

Intercodes translate_deflist(struct TreeNode* root);

Intercodes translate_def(struct TreeNode* root);

Intercodes translate_declist(struct TreeNode* root);

Intercodes translate_dec(struct TreeNode* root);

Intercodes translate_vardec(struct TreeNode* root);

Intercodes translate_exp(struct TreeNode* root, Operand place);

Intercodes cal_arrayAddr(struct TreeNode *root, Operand place);

Type getExpType(struct TreeNode* root);

Intercodes translate_args(struct TreeNode* root, Arg arg_list);

Intercodes cond_general(struct TreeNode *root, Operand place);

Intercodes translate_cond(struct TreeNode* root, Operand label_true, Operand label_false);

Intercodes translate_stmtlist(struct TreeNode* root);

Intercodes translate_stmt(struct TreeNode* root);
