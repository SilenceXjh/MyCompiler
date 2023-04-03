%{
    #include "tree.h"
    #include "lex.yy.c"
    #include <stdio.h>
    #define YYSTYPE struct TreeNode*
    extern int valid;
%}

%token INT FLOAT ID TYPE
%token SEMI COMMA ASSIGNOP RELOP
%token PLUS MINUS STAR DIV AND OR DOT NOT
%token LP RP LB RB LC RC
%token STRUCT RETURN IF ELSE WHILE

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE
%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right NOT
%left DOT LP RP LB RB

%%
Program : ExtDefList { $$ = create("Program", @$.first_line); 
    build1($$, $1); 
    setRoot($$); }
    ;
ExtDefList : { $$ = NULL; }
    | ExtDef ExtDefList { $$ = create("ExtDefList", @$.first_line); 
    build2($$, $1, $2); }
    ;
ExtDef : Specifier ExtDecList SEMI { $$ = create("ExtDef", @$.first_line); 
    build3($$, $1, $2, $3); }
    | Specifier SEMI { $$ = create("ExtDef", @$.first_line); 
    build2($$, $1, $2); }
    | Specifier FunDec CompSt { $$ = create("ExtDef", @$.first_line); 
    build3($$, $1, $2, $3); }
    ;
ExtDecList : VarDec { $$ = create("ExtDecList", @$.first_line); 
    build1($$, $1); }
    | VarDec COMMA ExtDecList { $$ = create("ExtDecList", @$.first_line); 
    build3($$, $1, $2, $3); }
    ;
Specifier : TYPE { $$ = create("Specifier", @$.first_line); 
    build1($$, $1); }
    | StructSpecifier { $$ = create("Specifier", @$.first_line); 
    build1($$, $1); }
    ;
StructSpecifier : STRUCT OptTag LC DefList RC { $$ = create("StructSpecifier", @$.first_line); 
    build5($$, $1, $2, $3, $4, $5); }
    | STRUCT Tag { $$ = create("StructSpecifier", @$.first_line); 
    build2($$, $1, $2); }
    ;
OptTag : ID { $$ = create("OptTag", @$.first_line); 
    build1($$, $1); }
    | { $$ = NULL; }
    ;
Tag : ID { $$ = create("Tag", @$.first_line); 
    build1($$, $1); }
    ;
VarDec : ID { $$ = create("VarDec", @$.first_line); 
    build1($$, $1); }
    | VarDec LB INT RB { $$ = create("VarDec", @$.first_line); 
    build4($$, $1, $2, $3, $4); }
    ;
FunDec : ID LP VarList RP { $$ = create("FunDec", @$.first_line); 
    build4($$, $1, $2, $3, $4); }
    | ID LP RP { $$ = create("FunDec", @$.first_line); 
    build3($$, $1, $2, $3); }
    ;
VarList : ParamDec COMMA VarList { $$ = create("VarList", @$.first_line); 
    build3($$, $1, $2, $3); }
    | ParamDec { $$ = create("VarList", @$.first_line); 
    build1($$, $1); }
    ;
ParamDec : Specifier VarDec { $$ = create("ParamDec", @$.first_line); 
    build2($$, $1, $2); }
    ;
CompSt : LC DefList StmtList RC { $$ = create("CompSt", @$.first_line); 
    build4($$, $1, $2, $3, $4); }
    ;
StmtList : Stmt StmtList { $$ = create("StmtList", @$.first_line); 
    build2($$, $1, $2); }
    | { $$ = NULL; }
    ;
Stmt : Exp SEMI { $$ = create("Stmt", @$.first_line); 
    build2($$, $1, $2); }
    | CompSt { $$ = create("Stmt", @$.first_line); 
    build1($$, $1); }
    | RETURN Exp SEMI { $$ = create("Stmt", @$.first_line); 
    build3($$, $1, $2, $3); }
    | IF LP Exp RP Stmt  %prec LOWER_THAN_ELSE { $$ = create("Stmt", @$.first_line); 
    build5($$, $1, $2, $3, $4, $5); }
    | IF LP Exp RP Stmt ELSE Stmt { $$ = create("Stmt", @$.first_line); 
    build7($$, $1, $2, $3, $4, $5, $6, $7); }
    | WHILE LP Exp RP Stmt { $$ = create("Stmt", @$.first_line); 
    build5($$, $1, $2, $3, $4, $5); }
    ;
DefList : Def DefList { $$ = create("DefList", @$.first_line); 
    build2($$, $1, $2); }
    | { $$ = NULL; }
    ;
Def : Specifier DecList SEMI { $$ = create("Def", @$.first_line); 
    build3($$, $1, $2, $3); }
    ;
DecList : Dec { $$ = create("DecList", @$.first_line); 
    build1($$, $1); }
    | Dec COMMA DecList { $$ = create("DecList", @$.first_line); 
    build3($$, $1, $2, $3); }
    ;
Dec : VarDec { $$ = create("Dec", @$.first_line); 
    build1($$, $1); }
    | VarDec ASSIGNOP Exp { $$ = create("Dec", @$.first_line); 
    build3($$, $1, $2, $3); }
    ;
Exp : Exp ASSIGNOP Exp { $$ = create("Exp", @$.first_line); 
    build3($$, $1, $2, $3); }
    | Exp AND Exp { $$ = create("Exp", @$.first_line); 
    build3($$, $1, $2, $3); }
    | Exp OR Exp { $$ = create("Exp", @$.first_line); 
    build3($$, $1, $2, $3); }
    | Exp RELOP Exp { $$ = create("Exp", @$.first_line); 
    build3($$, $1, $2, $3); }
    | Exp PLUS Exp { $$ = create("Exp", @$.first_line); 
    build3($$, $1, $2, $3); }
    | Exp MINUS Exp { $$ = create("Exp", @$.first_line); 
    build3($$, $1, $2, $3); }
    | Exp STAR Exp { $$ = create("Exp", @$.first_line); 
    build3($$, $1, $2, $3); }
    | Exp DIV Exp { $$ = create("Exp", @$.first_line); 
    build3($$, $1, $2, $3); }
    | LP Exp RP { $$ = create("Exp", @$.first_line); 
    build3($$, $1, $2, $3); }
    | MINUS Exp { $$ = create("Exp", @$.first_line); 
    build2($$, $1, $2); }
    | NOT Exp { $$ = create("Exp", @$.first_line); 
    build2($$, $1, $2); }
    | ID LP Args RP { $$ = create("Exp", @$.first_line); 
    build4($$, $1, $2, $3, $4); }
    | ID LP RP { $$ = create("Exp", @$.first_line); 
    build3($$, $1, $2, $3); }
    | Exp LB Exp RB { $$ = create("Exp", @$.first_line); 
    build4($$, $1, $2, $3, $4); }
    | Exp DOT ID { $$ = create("Exp", @$.first_line); 
    build3($$, $1, $2, $3); }
    | ID { $$ = create("Exp", @$.first_line); 
    build1($$, $1); }
    | INT { $$ = create("Exp", @$.first_line); 
    build1($$, $1); }
    | FLOAT { $$ = create("Exp", @$.first_line); 
    build1($$, $1); }
    ;
Args : Exp COMMA Args { $$ = create("Args", @$.first_line); 
    build3($$, $1, $2, $3); }
    | Exp { $$ = create("Args", @$.first_line); 
    build1($$, $1); }
    ;
Stmt : error SEMI {  } ;
CompSt : error RC {  } ;
%%
yyerror(char* msg) {
    valid = 0;
    printf("Error type B at Line %d: %s\n", yylineno, msg);
}