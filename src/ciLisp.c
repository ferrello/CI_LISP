// Name: Ferid Ruano
// Lab: CILisp - Task 7
// Date: 12/2/19
// Course: COMP 232 - Programming Languages

#include "ciLisp.h"


void yyerror(char *s) {
    // Note stderr that normally defaults to stdout, but can be redirected: ./src 2> src.log
    // CLion will display stderr in a different color from stdin and stdout
    fprintf(stderr, "\nERROR: %s\n", s);
    //exit(1);
}


RET_VAL eval(AST_NODE *node) {
    // RET_VAL (NUM_AST_NODE) with defaults
    RET_VAL result = {INT_TYPE, NAN};

    // Check for NULL AST_NODE
    if (NULL == node)
        return result;

    // Based on node's AST_NODE_TYPE perform equivalent evaluation
    switch (node->type) {
        case NUM_NODE_TYPE:
            result = evalNumNode(&node->data.number);
            break;
        case FUNC_NODE_TYPE:
            result = evalFuncNode(&node->data.function);
            break;
        case COND_NODE_TYPE:
            result = evalCondNode(&node->data.condition);
            break;
        case SYMBOL_NODE_TYPE:
            result = evalSymNode(node->data.symbol.ident, node);
            break;
        default:
            yyerror("Invalid AST_NODE_TYPE, probably invalid writes somewhere!");
    }

    return result;
}


RET_VAL evalNumNode(NUM_AST_NODE *number) {
    // RET_VAL (NUM_AST_NODE) with defaults
    RET_VAL result = {INT_TYPE, NAN};

    // Set RET_VAL's value, check for DOUBLE_TYPE
    result.value = number->value;
    if (DOUBLE_TYPE == number->type)
        result.type = DOUBLE_TYPE;

    return result;
}


RET_VAL evalFuncNode(FUNC_AST_NODE *funcNode) {
    // RET_VAL (NUM_AST_NODE) with defaults
    RET_VAL result = {INT_TYPE, NAN};

    // Check for NULL funcNode, before use
    if (NULL == funcNode)
        return result;

    checkParameters(funcNode->oper, funcNode->opList);

    RET_VAL op1 = eval(funcNode->opList);
    RET_VAL op2 = eval(funcNode->opList->next);

    switch (funcNode->oper) {
        case NEG_OPER:
            result.value = -op1.value;
            break;
        case ABS_OPER:
            result.value = fabs(op1.value);
            break;
        case SQRT_OPER:
            result.value = sqrt(op1.value);
            break;
        case EXP_OPER:
            result.value = exp(op1.value);
            break;
        case EXP2_OPER:
            result.value = exp2(op1.value);
            break;
        case LOG_OPER:
            if (result.value == 0.0)
                puts("Why you log a zero!?");
            result.value = log(op1.value);
            break;
        case CBRT_OPER:
            result.value = cbrt(op1.value);
            break;
        case SUB_OPER:
            result.value = op1.value - op2.value;
            break;
        case DIV_OPER:
            if (op2.value == 0.0)
                puts("Why you divide by zero!?");
            result.value = op1.value / op2.value;
            break;
        case REMAINDER_OPER:
            if (op2.value == 0.0)
                puts("Why you divide by zero!?");
            result.value = fmod(op1.value, op2.value);
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
        case HYPOT_OPER:
            result.value = hypot(op1.value, op2.value);
            break;
        case EQUAL_OPER:
            result.value = op1.value == op2.value ? 1 : 0;
            break;
        case LESS_OPER:
            result.value = op1.value < op2.value ? 1 : 0;
            break;
        case GREATER_OPER:
            result.value = op1.value > op2.value ? 1 : 0;
            break;
        case ADD_OPER:
        case MULT_OPER:
            result = multaryOperation(op1, funcNode->oper, funcNode->opList->next);
            break;
        case PRINT_OPER:
            result = printExpression(funcNode->opList);
            return result;
        case CUSTOM_OPER:
            result = customOperation(funcNode);
        default:
            // Returns {INT_TYPE, NAN}, should never occur
            break;
    }

    if (DOUBLE_TYPE == op1.type || DOUBLE_TYPE == op2.type)
        result.type = DOUBLE_TYPE;
    else
        result.value = floor(result.value);

    return result;
}


RET_VAL evalCondNode(COND_AST_NODE *condition) {
    RET_VAL result = eval(condition->cond);
    if (true == result.value)
        return eval(condition->valid);
    else
        return eval(condition->invalid);
}


RET_VAL evalSymNode(char *ident, AST_NODE *parent) {
    // Base case, symbol not found in outer scopes
    if (parent == NULL) {
        message(2, ident);
        return (RET_VAL) {INT_TYPE, NAN};
    }

    SYMBOL_TABLE_NODE *currSymTabNode = parent->symbolTable;

    // Search current scope for symbol
    while (currSymTabNode != NULL) {
        if (strcmp(ident, currSymTabNode->ident) == 0) {
            RET_VAL result = eval(currSymTabNode->value);

            // Check for a data type. If there is a data type, then assure
            // that the AST_NODE value has the same explicit data type provided.
            if (INT_TYPE == currSymTabNode->dataType && DOUBLE_TYPE == result.type) {
                // Throw a warning when explicitly converting a DOUBLE_TYPE value to INT_TYPE
                message(3, currSymTabNode->ident);
                // Floor the double value for INT_TYPE
                result.value = floor(result.value);
                // Set result's type as the explicit data type
                result.type = currSymTabNode->dataType;
            }
            if (DOUBLE_TYPE == currSymTabNode->dataType && INT_TYPE == result.type)
                result.type = DOUBLE_TYPE;
            return result;
        }
        // Move to next SYMBOL_TABLE_NODE
        currSymTabNode = currSymTabNode->next;
    }

    // Recursively search parent's symbol table
    return evalSymNode(ident, parent->parent);
}


RET_VAL multaryOperation(RET_VAL result, OPER_TYPE operation, AST_NODE *opList) {
    AST_NODE *currOp = opList;
    while (NULL != currOp) {
        RET_VAL op = eval(currOp);
        switch (operation) {
            case ADD_OPER:
                result.value += op.value;
                break;
            case MULT_OPER:
                result.value *= op.value;
                break;
            default:
                break;
        }

        // Check for DOUBLE_TYPE
        if (DOUBLE_TYPE == op.type)
            result.type = DOUBLE_TYPE;

        // Move to the next opList AST_NODE
        currOp = currOp->next;
    }

    return result;
}


RET_VAL printExpression(AST_NODE *opList) {
    RET_VAL result = {INT_TYPE, NAN};

    printf("%s", "=> ");
    AST_NODE *currOpNode = opList;
    while (NULL != currOpNode) {
        result = eval(currOpNode);
        switch (result.type) {
            case INT_TYPE:
                printf("%ld ", (long) result.value);
                break;
            case DOUBLE_TYPE:
                printf("%lf ", result.value);
                break;
            default:
                break;
        }

        currOpNode = currOpNode->next;
    }
    puts("");

    return result;
}


RET_VAL customOperation(FUNC_AST_NODE *function) {
    RET_VAL result = {INT_TYPE, NAN};

    // Lambda Function Definition
    SYMBOL_TABLE_NODE *lambda = evalLambda(function->ident, function->opList);

    if (NULL == lambda)
        return result;

    SYMBOL_TABLE_NODE *currArg = lambda->value->argumentTable;
    AST_NODE *currOp = function->opList;
    while (NULL != currOp) {
        result = eval(currOp);
        currArg->value = initASTNode();
        currArg->value->type = NUM_NODE_TYPE;
        currArg->value->data.number.value = result.value;
        currArg->value->data.number.type = result.type;
        currArg = currArg->next;
        currOp = currOp->next;
    }

    result = eval(lambda->value);

    return result;
}


SYMBOL_TABLE_NODE *evalLambda(char *ident, AST_NODE *parent) {
    // Base case, lambda not found in outer scopes
    if (parent == NULL) {
        message(4, ident);
        return NULL;
    }

    SYMBOL_TABLE_NODE *currArgTabNode = parent->symbolTable;

    // Search current scope for lambda
    while (NULL != currArgTabNode) {
        if (strcmp(ident, currArgTabNode->ident) == 0) {
            return currArgTabNode;
        }
        currArgTabNode = currArgTabNode->next;
    }

    // Recursively search parent's symbol table
    return evalLambda(ident, parent->parent);
}


void checkParameters(OPER_TYPE operation, AST_NODE *opList) {

    switch (operation) {
        case NEG_OPER ... CBRT_OPER:
            if (NULL != opList->next)
                message(1, funcNames[operation]);
            break;
        case SUB_OPER ... GREATER_OPER:
            if (opList->next == NULL)
                message(0, funcNames[operation]);
            else if (opList->next->next != NULL)
                message(1, funcNames[operation]);
            break;
        case ADD_OPER:
        case MULT_OPER:
        case CUSTOM_OPER:
            if (opList->next == NULL)
                message(0, funcNames[operation]);
            break;
        default:
            break;
    }
}


void printRetVal(RET_VAL result) {
    // Based on NUM_TYPE of RET_VAL print accordingly
    if (isnan(result.value) == 0) {
        switch (result.type) {
            case INT_TYPE:
                printf("%ld\n", (long) result.value);
                break;
            case DOUBLE_TYPE:
                printf("%lf\n", result.value);
                break;
            default:
                break;
        }
    }
}


void freeNode(AST_NODE *node) {
    if (NULL == node)
        return;

    switch (node->type) {
        case NUM_NODE_TYPE:
            break;
        case FUNC_NODE_TYPE:
            freeNode(node->data.function.opList->next);
            free(node->data.function.ident);
            freeNode(node->data.function.opList);
            break;
        case COND_NODE_TYPE:
            freeNode(node->data.condition.cond);
            freeNode(node->data.condition.valid);
            freeNode(node->data.condition.invalid);
            break;
        case SYMBOL_NODE_TYPE:
            free(node->data.symbol.ident);
            break;
    }

    freeSymbolTable(node->symbolTable);
    freeSymbolTable(node->argumentTable);
    free(node);
}


void freeSymbolTable(SYMBOL_TABLE_NODE *node) {
    if (NULL == node)
        return;

//    free(node->ident);
//    freeNode(node->value);
//    freeSymbolTable(node->next);
//    free(node);
}


void message(short type, char *data) {
    switch (type) {
        case 0:
            printf("Error :: Too few parameters for the function \"%s\". "
                   "First parameter will be passed to calculate result!\n", data);
            break;
        case 1:
            printf("Warning :: Too many parameters for the function \"%s\"\n", data);
            break;
        case 2:
            printf("Error :: Symbol not found in scope \"%s\"\n", data);
            break;
        case 3:
            printf("Warning :: Precision loss in the assignment for variable \"%s\"\n", data);
            break;
        case 4:
            printf("Error :: Lambda not found in scope \"%s\"\n", data);
            break;
        default:
            break;
    }
}