#include "ciLisp.h"

void yyerror(char *s) {
    fprintf(stderr, "\nERROR: %s\n", s);
    // Note stderr that normally defaults to stdout, but can be redirected: ./src 2> src.log
    // CLion will display stderr in a different color from stdin and stdout
}


// Array of string values for operations.
// Must be in sync with funcs in the OPER_TYPE enum in order for resolveFunc to work.
char *funcNames[] = {
        "neg",
        "abs",
        "exp",
        "sqrt",
        "add",
        "sub",
        "mult",
        "div",
        "remainder",
        "log",
        "pow",
        "max",
        "min",
        "exp2",
        "cbrt",
        "hypot",
        "read",
        "rand",
        "print",
        "equal",
        "less",
        "greater",
        ""};


OPER_TYPE resolveFunc(char *funcName) {
    int i = 0;
    while (funcNames[i][0] != '\0') {
        if (strcmp(funcNames[i], funcName) == 0)
            return i;
        i++;
    }
    return CUSTOM_OPER;
}


AST_NODE *createNumberNode(double value, NUM_TYPE type) {
    AST_NODE *node = NULL;
    size_t nodeSize = sizeof(AST_NODE);

    // Allocate space for AST_NODE, otherwise error
    if ((node = calloc(1, nodeSize)) == NULL)
        yyerror("Memory allocation failed!");

    // TODO - Well Done Fredward !
    node->type = NUM_NODE_TYPE;         // Set AST_NODE's type
    node->data.number.type = type;      // Set NUM_AST_NODE's type
    node->data.number.value = value;    // Set NUM_AST_NODE's value
    return node;
}

AST_NODE *createFunctionNode(char *funcName, AST_NODE *op1, AST_NODE *op2) {
    AST_NODE *node = NULL;
    size_t nodeSize = sizeof(AST_NODE);

    // Allocate space for AST_NODE, otherwise error
    if ((node = calloc(1, nodeSize)) == NULL)
        yyerror("Memory allocation failed!");

    // TODO - Well Done Fredward !
    node->type = FUNC_NODE_TYPE;        // Set AST_NODE's type
    node->data.function.op1 = op1;      // Point FUNC_AST_NODE's op1
    node->data.function.op2 = op2;      // Point FUNC_AST_NODE's op2

    // Set FUNC_AST_NODE's operator, check for custom operator
    // Free funcName for built-in operators
    node->data.function.oper = resolveFunc(funcName);
    if (CUSTOM_OPER == node->data.function.oper)
        node->data.function.ident = funcName;
    else
        free(funcName);

    return node;
}

// Called after execution is done on the base of the tree.
// (see the program production in ciLisp.y)
// Recursively frees the whole abstract syntax tree.
// You'll need to update and expand freeNode as the project develops.
void freeNode(AST_NODE *node) {
    if (!node)
        return;

    if (node->type == FUNC_NODE_TYPE) {
        // Recursive calls to free child nodes
        freeNode(node->data.function.op1);
        freeNode(node->data.function.op2);

        // Free up identifier string if necessary
        if (node->data.function.oper == CUSTOM_OPER) {
            free(node->data.function.ident);
        }
    }

    free(node);
}


// Evaluates an AST_NODE.
// Returns a RET_VAL storing the the resulting value and type.
RET_VAL eval(AST_NODE *node) {
    // RET_VAL (NUM_AST_NODE) with defaults
    RET_VAL result = {INT_TYPE, NAN};

    // Check for NULL AST_NODE
    if (!node)
        return result;

    // TODO - Well Done Fredward !

    // Based on node's AST_NODE_TYPE perform equivalent eval
    switch (node->type) {
        case NUM_NODE_TYPE:
            result = evalNumNode(node);
            break;
        case FUNC_NODE_TYPE:
            result = evalFuncNode(node);
            break;
        default:
            yyerror("Invalid AST_NODE_TYPE, probably invalid writes somewhere!");
    }

    return result;
}


// Returns a pointer to the NUM_AST_NODE (aka RET_VAL) referenced by node.
// Does not allocate space for a new RET_VAL.
RET_VAL evalNumNode(AST_NODE *numNode) {
    // RET_VAL (NUM_AST_NODE) with defaults
    RET_VAL result = {INT_TYPE, NAN};

    // Check for NULL numNode
    if (!numNode)
        return result;

    // TODO - Well Done Fredward !

    // Set RET_VAL's value, check for DOUBLE_TYPE
    result.value = numNode->data.number.value;
    if (DOUBLE_TYPE == numNode->data.number.type)
        result.type = DOUBLE_TYPE;

    return result;
}


RET_VAL evalFuncNode(AST_NODE *funcNode) {
    // RET_VAL (NUM_AST_NODE) with defaults
    RET_VAL result = {INT_TYPE, NAN};

    // Check for NULL funcNode, before use
    if (!funcNode)
        return result;

    RET_VAL op1 = eval(funcNode->data.function.op1); // RET_VAL for funcNode's first operand
    RET_VAL op2 = eval(funcNode->data.function.op2); // RET_VAL for funcNode's second operand

    // Set NUM_TYPE to DOUBLE_TYPE when at least one double operand exists
    if (DOUBLE_TYPE == op1.type || DOUBLE_TYPE == op2.type)
        result.type = DOUBLE_TYPE;

    // TODO - Well Done Fredward !

    // Based on funcNode's operator perform equivalent action
    switch (funcNode->data.function.oper) {
        case NEG_OPER:
            result.value = -op1.value;
            break;
        case ABS_OPER:
            result.value = fabs(op1.value);
            break;
        case EXP_OPER:
            result.value = exp(op1.value);
        case SQRT_OPER:
            result.value = sqrt(op1.value);
            break;
        case ADD_OPER:
            result.value = op1.value + op2.value;
            break;
        case SUB_OPER:
            result.value = op1.value - op2.value;
            break;
        case MULT_OPER:
            result.value = op1.value * op2.value;
            break;
        case DIV_OPER:
            result.value = op1.value / op2.value;
            break;
        case REMAINDER_OPER:
            result.value = fmod(op1.value, op2.value);
            break;
        case LOG_OPER:
            result.value = log(op1.value);
            break;
        case POW_OPER:
            result.value = pow(op1.value, op2.value);
            break;
        case MAX_OPER:
            result.value = op1.value > op2.value ? op1.value : op2.value;
            break;
        case MIN_OPER:
            result.value = op1.value < op2.value ? op1.value : op2.value;
            break;
        case EXP2_OPER:
            result.value = op1.value;
            break;
        case CBRT_OPER:
            result.value = cbrt(op1.value);
            break;
        case HYPOT_OPER:
            result.value = hypot(op1.value, op2.value);
            break;
        default:
            break;
    }

    return result;
}

void printRetVal(RET_VAL val) {
    // TODO - Well Done Fredward !

    // Based on NUM_TYPE of RET_VAL print accordingly
    switch (val.type) {
        case INT_TYPE:
            printf("RET_VAL :: %s :: %ld\n", "INT_TYPE", (long) val.value);
            break;
        case DOUBLE_TYPE:
            printf("RET_VAL :: %s :: %lf\n", "DOUBLE_TYPE", val.value);
            break;
    }
}
