%{
    #include "tree.h"
    #define YYSTYPE struct TreeNode*
    #include "syntax.tab.h"
    #include <stdlib.h>
    #include <string.h>
    int yycolumn = 1;
    #define YY_USER_ACTION \
        yylloc.first_line = yylloc.last_line = yylineno; \
        yylloc.first_column = yycolumn; \
        yylloc.last_column = yycolumn + yyleng - 1; \
        yycolumn += yyleng;
    extern int valid;
%}

digit [0-9]
letter [_a-zA-Z]
ordifloat {digit}+\.{digit}+
base {digit}*\.{digit}+|{digit}+\.{digit}*
expfloat {base}[Ee][+-]?{digit}+
%option yylineno
%%
" "|\t { }
\n { yycolumn = 1; }
0|[1-9]{digit}* { 
    yylval = createLeaf("INT");
    yylval->type = 3;
    yylval->extraInfo.valInt = atoi(yytext);
    return INT;
}
{ordifloat}|{expfloat} { 
    yylval = createLeaf("FLOAT");
    yylval->type = 4;
    yylval->extraInfo.valFloat = atof(yytext);
    return FLOAT;
}
";" { yylval = createLeaf("SEMI"); return SEMI; }
"," { yylval = createLeaf("COMMA"); return COMMA; }
"=" { yylval = createLeaf("ASSIGNOP"); return ASSIGNOP; }
>|<|>=|<=|==|!= { 
    yylval = createLeaf("RELOP"); 
    strcpy(yylval->extraInfo.relop, yytext);
    return RELOP; 
}
"+" { yylval = createLeaf("PLUS"); return PLUS; }
"-" { yylval = createLeaf("MINUS"); return MINUS; }
"*" { yylval = createLeaf("STAR"); return STAR; }
"/" { yylval = createLeaf("DIV"); return DIV; }
"&&" { yylval = createLeaf("AND"); return AND; }
"||" { yylval = createLeaf("OR"); return OR; }
"." { yylval = createLeaf("DOT"); return DOT; }
"!" { yylval = createLeaf("NOT"); return NOT; }
int { 
    yylval = createLeaf("TYPE"); 
    yylval->type = 2;
    yylval->extraInfo.iof = 0;
    return TYPE;
}
float {
    yylval = createLeaf("TYPE"); 
    yylval->type = 2;
    yylval->extraInfo.iof = 1;
    return TYPE;
}
"(" { yylval = createLeaf("LP"); return LP; }
")" { yylval = createLeaf("RP"); return RP; }
"[" { yylval = createLeaf("LB"); return LB; }
"]" { yylval = createLeaf("RB"); return RB; }
"{" { yylval = createLeaf("LC"); return LC; }
"}" { yylval = createLeaf("RC"); return RC; }
struct { yylval = createLeaf("STRUCT"); return STRUCT; }
return { yylval = createLeaf("RETURN"); return RETURN; }
if { yylval = createLeaf("IF"); return IF; }
else { yylval = createLeaf("ELSE"); return ELSE; }
while { yylval = createLeaf("WHILE"); return WHILE; }
{letter}({digit}|{letter})* { 
    yylval = createLeaf("ID");
    yylval->type = 1;
    strcpy(yylval->extraInfo.word, yytext);
    return ID;
}
. { printf("Error type A at Line %d: Mysterious character \'%s\'\n", yylineno, yytext);
    valid = 0; }
%%
