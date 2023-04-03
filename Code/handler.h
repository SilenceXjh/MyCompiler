void handle_extdef(struct TreeNode *root);

void handle_extdeclist(Type base, struct TreeNode *root);

void handle_vardec_global(Type cur, struct TreeNode *root);

Type handle_specifier(struct TreeNode *root);

int handle_struct(struct TreeNode *root);

void handle_structDefList(FieldList head, struct TreeNode *root);

void add_structDef(FieldList head, struct TreeNode *root);

void add_structDecList(FieldList head, Type _t, struct TreeNode *root);

void add_structDec(FieldList head, Type _t, struct TreeNode *root);

void add_structVarDec(FieldList head, Type _t, struct TreeNode *root);

void handle_funDec(Type t, struct TreeNode *root);

void handle_valList(FieldList params, struct TreeNode *root);

void add_paramDec(FieldList params, struct TreeNode *root);

void add_paramVarDec(FieldList head, Type _t, struct TreeNode *root);

void handle_compSt(Type ret, struct TreeNode *root);

void deleteCurLevel(Var head);

void handle_defList(struct TreeNode *root);

void handle_def(struct TreeNode *root);

void handle_decList(Type _t, struct TreeNode *root);

void handle_dec(Type _t, struct TreeNode *root);

void handle_varDec(Type _t, struct TreeNode *root);

Type handle_exp(struct TreeNode *root);

void judge_args(FieldList params, struct TreeNode *root);

void judgeLeftValue(struct TreeNode *root);

void handle_stmtList(Type ret, struct TreeNode *root);

void handle_stmt(Type ret, struct TreeNode *root);