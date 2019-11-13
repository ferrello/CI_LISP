// Name: Ferid Ruano
// Lab: ciLisp
// Date: 10/30/19
// Course: COMP 232 - Programming Languages


// ******************
//	Prologue
// ******************
// The Prologue section contains macro definitions and declarations
// of functions and variables that are used in the actions in the
// grammar rules.
%{
    #include "ciLisp.h"
%}

// ****************************
//	Bison Declarations
// ****************************
// Bison declarations section contains declarations that define terminal
// and nonterminal symbols, specify precedence, and so on.

// Bison Manual - 3.7.2, 3.7.4
// %token - terminal symbols
// %type - non-terminal symbols (no precedence or associativity)
// %union - yylval members (i.e yylval.dval) and semantic value <data_type>
// <data_type> - Used to hold semantic value
// No <data_type> - Does not hold semantic value (literal)

%union {
    double dval;
    char *sval;
    struct ast_node *astNode;
}

// Terminal symbols with a semantic value
%token <sval> FUNC
%token <dval> INT DOUBLE

// Terminal symbols without semantic value
%token LPAREN RPAREN EOL QUIT

// Non-terminal sybmols with semantic value
%type <astNode> s_expr f_expr number


// ***********************
//	Grammer Rules
// ***********************
%%

program:
    s_expr EOL {
        fprintf(stderr, "yacc: program ::= s_expr EOL\n");
        if ($1) {
            printRetVal(eval($1));
            freeNode($1);
        }
    };

s_expr:
    number {
        fprintf(stderr, "yacc: s_expr ::= number\n");
        $$ = $1;
    }
    | f_expr {
        $$ = $1;
    }
    | QUIT {
        fprintf(stderr, "yacc: s_expr ::= QUIT\n");
        exit(EXIT_SUCCESS);
    }
    | error {
        fprintf(stderr, "yacc: s_expr ::= error\n");
        yyerror("unexpected token");
        $$ = NULL;
    };

number:
    INT {
        fprintf(stderr, "yacc: number ::= INT\n");
        $$ = createNumberNode($1, INT_TYPE);
    }
    | DOUBLE {
        fprintf(stderr, "yaf_exprcc: number ::= DOUBLE\n");
        $$ = createNumberNode($1, DOUBLE_TYPE);
    };

f_expr:
    LPAREN FUNC s_expr RPAREN {
        fprintf(stderr, "yacc: s_expr ::= LPAREN FUNC s_expr RPAREN\n");
        $$ = createFunctionNode($2, $3, NULL);
    }
    | LPAREN FUNC s_expr s_expr RPAREN {
        fprintf(stderr, "yacc: s_expr ::= LPAREN FUNC s_expr s_expr RPAREN\n");
        $$ = createFunctionNode($2, $3, $4);
    };

%%


// ******************
//	Epilogue
// ******************
// This is the most convenient place to put anything that you want to
// have in the parser implementation file but which need not come before
// the definition of yyparse. For example, the definitions of yylex and
// yyerror often go here.