// Name: Ferid Ruano
// Lab: CILisp - Task 7
// Date: 12/6/19
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
// %token - Terminal symbols
// %type - Non-terminal symbols (no precedence or associativity)
// %union - yylval members (i.e yylval.dval) and semantic value <data_type>
// <data_type> - Used to hold semantic value
// No <data_type> - Does not hold semantic value (literal)


// Semantic value data types
%union {
    double dval;
    char *sval;
    short type;
    struct ast_node *astNode;
    struct symbol_table_node *symTabNode;
}

// Terminal symbols with a semantic value
%token <sval> FUNC SYMBOL
%token <dval> INT_LITERAL DOUBLE_LITERAL

// Terminal symbols without semantic value
%token LPAREN RPAREN INT DOUBLE LET COND LAMBDA EOL QUIT

// Non-terminal sybmols with semantic value
%type <astNode> s_expr s_expr_list
%type <symTabNode> let_elem let_list let_section arg_list
%type <type> type


// ***********************
//	Grammer Rules
// ***********************
// Bison Manual - 5.0, 5.1, 5.2, 8.0
// cmake-build-debug/ciLispParer.output

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
	INT_LITERAL {
	    	fprintf(stderr, "yacc: number ::= INT\n");
		AST_NODE *node = initASTNode();
		node->type = NUM_NODE_TYPE;
		node->data.number.type = INT_TYPE;
		node->data.number.value = $1;
		$$ = node;
	}
	| DOUBLE_LITERAL {
	    	fprintf(stderr, "yacc: number ::= DOUBLE\n");
		AST_NODE *node = initASTNode();
		node->type = NUM_NODE_TYPE;
		node->data.number.type = DOUBLE_TYPE;
		node->data.number.value = $1;
		$$ = node;
	}
    	| LPAREN FUNC RPAREN {
		fprintf(stderr, "yacc: s_expr ::= LPAREN FUNC RPAREN\n");
		AST_NODE *node = initASTNode();
		node->data.function.oper = resolveFunc($2);
		if (READ_OPER == node->data.function.oper) {
			double value = 0.0;
			char buffer[BUF_LEN];
			NUM_TYPE type = INT_TYPE;
			printf("%s", "Read := ");
			fgets(buffer, BUF_LEN, stdin);
			if (NULL != strchr(buffer, '.'))
				type = DOUBLE_TYPE;
			value = strtod(buffer, NULL);
			node->type = NUM_NODE_TYPE;
			node->data.number.type = type;
			node->data.number.value = value;
		}
		else if (RAND_OPER == node->data.function.oper) {
			node->type = NUM_NODE_TYPE;
			node->data.number.type = DOUBLE_TYPE;
			node->data.number.value = rand() / (double) RAND_MAX;
		} else {
			free(node);
			node = NULL;
		}
		free($2);
		$$ = node;
	}
	| LPAREN FUNC s_expr_list RPAREN {
		fprintf(stderr, "yacc: s_expr ::= LPAREN FUNC s_expr_list RPAREN\n");
		AST_NODE *node = initASTNode();
		node->type = FUNC_NODE_TYPE;
		node->data.function.oper = resolveFunc($2);
		node->data.function.opList = $3;
		AST_NODE *currOpNode = $3;
		while (NULL != currOpNode) {
			currOpNode->parent = node;
			currOpNode = currOpNode->next;
		}
		free($2);
		$$ = node;
	}
	| LPAREN SYMBOL s_expr_list RPAREN {
		fprintf(stderr, "yacc: f_expr ::= LPAREN SYMBOL s_expr_list RPAREN\n");
		AST_NODE *node = initASTNode();
		node->type = FUNC_NODE_TYPE;
		node->data.function.oper = resolveFunc($2);
		node->data.function.ident = $2;
		node->data.function.opList = $3;
		AST_NODE *currOpNode = $3;
		while (NULL != currOpNode) {
			currOpNode->parent = node;
			currOpNode = currOpNode->next;
		}
		$$ = node;
	}
    	| SYMBOL {
		fprintf(stderr, "yacc: s_expr ::= SYMBOL\n");
		AST_NODE *node = initASTNode();
		node->type = SYMBOL_NODE_TYPE;
		node->data.symbol.ident = $1;
		$$ = node;
    	}
    	| LPAREN let_section s_expr RPAREN {
		fprintf(stderr, "yacc: s_expr ::= LPAREN let_section s_expr RPAREN\n");
		if (NULL == $2)
			$$ = $3;
		if (NULL == $3)
			yyerror("Invalid input! Parent AST_NODE is NULL when scoping symbol table.");
		$3->symbolTable = $2;
		SYMBOL_TABLE_NODE *currNode = $2;
		while (NULL != currNode) {
			if (NULL != currNode->value)
				currNode->value->parent = $3;
		currNode = currNode->next;
		}
		$$ = $3;
	}
	| LPAREN COND s_expr s_expr s_expr RPAREN {
		fprintf(stderr, "yacc: s_expr ::= LPAREN COND s_expr s_expr s_expr RPAREN\n");
		AST_NODE *node = initASTNode();
		node->type = COND_NODE_TYPE;
		node->data.condition.valid = $4;
		node->data.condition.cond = $3;
		node->data.condition.invalid = $5;
		$4->parent = node;
		$3->parent = node;
		$5->parent = node;
		$$ = node;
	}
	| QUIT {
		fprintf(stderr, "yacc: s_expr ::= QUIT\n");
		exit(EXIT_SUCCESS);
	}
	| error {
		fprintf(stderr, "yacc: s_expr ::= error\n");
		yyerror("Unexpected token");
		$$ = NULL;
	};


s_expr_list:
	/* empty */ {
		$$ = NULL;
	}
	| s_expr s_expr_list {
		AST_NODE *currOpNode = $1;
		while (NULL != currOpNode->next)
		    currOpNode = currOpNode->next;
		currOpNode->next = $2;
		$$ = $1;
	};


type:
	/* empty */ {
		$$ = NO_TYPE;
	}
	| INT {
		$$ = INT_TYPE;
	}
	| DOUBLE {
		$$ = DOUBLE_TYPE;
	};


let_section:
	/* empty */ {
		fprintf(stderr, "yacc: let_section ::= <empty>\n");
		$$ = NULL;
	}
	| LPAREN LET let_list RPAREN {
		fprintf(stderr, "yacc: let_section ::= LPAREN LET let_list RPAREN\n");
		$$ = $3;
	};


let_list:
	let_elem {
		fprintf(stderr, "yacc: let_list ::= let_elem\n");
		$$ = $1;
	}
	| let_list let_elem  {
		fprintf(stderr, "yacc: let_list ::= let_list let_elem\n");
		$2->next = $1;
		$$ = $2;
	};


let_elem:
	LPAREN type SYMBOL s_expr RPAREN {
		fprintf(stderr, "yacc: let_elem ::= LPAREN type symbol s_expr RPAREN\n");
		SYMBOL_TABLE_NODE *node = initSTNode();
		node->type = VARIABLE_TYPE;
		node->dataType = $2;
		node->ident = $3;
		node->value = $4;
		$$ = node;
	}
	| LPAREN type SYMBOL LAMBDA LPAREN arg_list RPAREN s_expr RPAREN {
		fprintf(stderr, "yacc: let_elem ::= LPAREN type SYMBOL LAMBDA LPAREN arg_list RPAREN s_expr RPAREN\n");
		SYMBOL_TABLE_NODE *node = initSTNode();
		node->type = LAMBDA_TYPE;
		node->dataType = $2;
		node->ident = $3;
		node->value = $8;
		node->next = $6;
		node->value->argumentTable = $6;
		$$ = node;
	};


arg_list:
	SYMBOL {
		fprintf(stderr, "yacc: arg_list ::= SYMBOL\n");
		SYMBOL_TABLE_NODE *node = initSTNode();
		node->type = ARG_TYPE;
		node->dataType = NO_TYPE;
		node->ident = $1;
		$$ = node;
	}
	| SYMBOL arg_list {
		fprintf(stderr, "yacc: arg_list ::= SYMBOL arg_list\n");
		SYMBOL_TABLE_NODE *node = initSTNode();
		node->type = ARG_TYPE;
		node->dataType = NO_TYPE;
		node->ident = $1;
		node->next = $2;
		$$ = node;
	};


%%


// ******************
//	Epilogue
// ******************
// This is the most convenient place to put anything that you want to
// have in the parser implementation file but which need not come before
// the definition of yyparse. For example, the definitions of yylex and
// yyerror often go here.