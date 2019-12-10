// Name: Ferid Ruano
// Lab: CILisp - Task 7
// Date: 12/2/19
// Course: COMP 232 - Programming Languages

#ifndef __cilisp_h_
#define __cilisp_h_


// ******************
//     PreProcess
// ******************

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include "ciLispParser.h"

#define BUF_LEN 64

// ******************
//	   Flex/Bison
// ******************

int yylex(void);

void yyerror(char *);

static char *funcNames[] = {
        "neg",
        "abs",
        "sqrt",
        "exp",
        "exp2",
        "log",
        "cbrt",
        "sub",
        "div",
        "remainder",
        "pow",
        "max",
        "min",
        "hypot",
        "equal",
        "less",
        "greater",
        "add",
        "mult",
        "print",
        "read",
        "rand",
        ""};

// Enum of all AST_FUNCTION_NODE operators.
// Must be in sync with funcs in resolveFunc()
typedef enum oper {
    NEG_OPER,
    ABS_OPER,
    SQRT_OPER,
    EXP_OPER,
    EXP2_OPER,
    LOG_OPER,
    CBRT_OPER,
    SUB_OPER,
    DIV_OPER,
    REMAINDER_OPER,
    POW_OPER,
    MAX_OPER,
    MIN_OPER,
    HYPOT_OPER,
    EQUAL_OPER,
    LESS_OPER,
    GREATER_OPER,
    ADD_OPER,
    MULT_OPER,
    PRINT_OPER,
    READ_OPER,
    RAND_OPER,
    CUSTOM_OPER = 255
} OPER_TYPE;


// ******************
//	 Abstract Tree
// ******************

// Types of Abstract Syntax Tree nodes.
typedef enum {
    NUM_NODE_TYPE,
    FUNC_NODE_TYPE,
    COND_NODE_TYPE,
    SYMBOL_NODE_TYPE
} AST_NODE_TYPE;

// Types of numeric values
typedef enum {
    NO_TYPE,
    INT_TYPE,
    DOUBLE_TYPE
} NUM_TYPE;

// Types of symbol type
typedef enum {
    VARIABLE_TYPE,
    LAMBDA_TYPE,
    ARG_TYPE
} SYMBOL_TYPE;

// Node to store a number.
typedef struct {
    NUM_TYPE type;
    double value;
} NUM_AST_NODE;

// RET_VAL with NUM_AST_NODE structure for eval return types (readability)
typedef NUM_AST_NODE RET_VAL;

// Node to store a function call with its inputs
typedef struct {
    OPER_TYPE oper;                             // ENUM Type
    char *ident;                                // Custom function identifier
    struct ast_node *opList;                    // Linked-list of arguments
} FUNC_AST_NODE;

// Node to store a symbol table (LL) node with its data
typedef struct symbol_table_node {
    SYMBOL_TYPE type;                           // VAR, ARG, LAMBDA Type
    NUM_TYPE dataType;                          // INT_TYPE or DOUBLE_TYPE
    char *ident;                                // Symbol Identifier
    struct ast_node *value;                     // Any AST_NODE_TYPE value
    struct symbol_table_node *next;             // Pointer to next symbol table (LL) node
} SYMBOL_TABLE_NODE;

// Node to store a symbol with its identifier
typedef struct symbol_ast_node {
    char *ident;
} SYMBOL_AST_NODE;

typedef struct cond_ast_node {
    struct ast_node *cond;                      // Type of condition: Read, Rand, Greater, Less, Equal
    struct ast_node *valid;                     // To eval if a condition is non-zero
    struct ast_node *invalid;                   // To eval if a condition is zero
} COND_AST_NODE;

// Generic Abstract Syntax Tree node. Stores the type of node,
// and reference to the corresponding specific node.
typedef struct ast_node {
    AST_NODE_TYPE type;                         // NUM, FUNC, SYMBOL
    SYMBOL_TABLE_NODE *symbolTable;             // AST_NODE holds a scoped symbol table (LL)
    SYMBOL_TABLE_NODE *argumentTable;           // AST_NODE holds a scoped symbol table (LL)
    struct ast_node *parent;                    // AST_NODE points to a parent AST_NODE (for outer scope)
    union {
        NUM_AST_NODE number;                    // Holds NUM_AST_NODE to store a number
        FUNC_AST_NODE function;                 // Holds FUNC_AST_NODE to store an operation
        COND_AST_NODE condition;                // Holds a COND_AST_NODE to store a condition
        SYMBOL_AST_NODE symbol;                 // Holds SYMBOL_AST_NODE to reference a symbol
    } data;
    struct ast_node *next;
} AST_NODE;


// ******************
//     Functions
// ******************

RET_VAL eval(AST_NODE *);

RET_VAL evalNumNode(NUM_AST_NODE *);

RET_VAL evalFuncNode(FUNC_AST_NODE *);

RET_VAL evalCondNode(COND_AST_NODE *);

RET_VAL evalSymNode(char *, AST_NODE *);

RET_VAL multaryOperation(RET_VAL, OPER_TYPE, AST_NODE *);

RET_VAL printExpression(AST_NODE *);

RET_VAL customOperation(FUNC_AST_NODE *);

SYMBOL_TABLE_NODE *evalLambda(char *, AST_NODE *);

void checkParameters(OPER_TYPE, AST_NODE *);

void printRetVal(RET_VAL);

void freeNode(AST_NODE *);

void freeSymbolTable(SYMBOL_TABLE_NODE *);

void message(short, char *);

static inline AST_NODE *initASTNode() {
    AST_NODE *node = NULL;

    // Allocate space for AST_NODE, otherwise error
    if ((node = calloc(1, sizeof(AST_NODE))) == NULL)
        yyerror("Memory allocation failed!");

    // Note: For real world systems, calloc will zero out (point to NULL)
    // every field of the AST_NODE.

    return node;
}

static inline SYMBOL_TABLE_NODE *initSTNode() {
    SYMBOL_TABLE_NODE *node = NULL;

    // Allocate space for AST_NODE, otherwise error
    if ((node = calloc(1, sizeof(SYMBOL_TABLE_NODE))) == NULL)
        yyerror("Memory allocation failed!");

    // Note: For real world systems, calloc will zero out (point to NULL)
    // every field of the SYMBOL_TABLE_NODE.

    return node;
}

static inline OPER_TYPE resolveFunc(char *funcName) {
    int i = 0;
    while (funcNames[i][0] != '\0') {
        if (strcmp(funcNames[i], funcName) == 0)
            return i;
        i++;
    }
    return CUSTOM_OPER;
}

#endif