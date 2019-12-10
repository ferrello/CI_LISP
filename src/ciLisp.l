/* Name: Ferid Ruano
 * Lab: CILisp - Task 7
 * Date: 12/2/19
 * Course: COMP 232 - Programming Languages
 */


 /*****************/
 /*  Definitions  */
 /*****************/

%option noyywrap
%option nounput
%option noinput

%{
    #include "ciLisp.h"
%}

digit [0-9]
int [+-]?{digit}+
double [+-]?{digit}+[.]{digit}*
symbol [a-zA-z]+
func "neg"|"abs"|"add"|"sub"|"mult"|"remainder"|"pow"|"max"|"min"|"sqrt"|"exp"|"exp2"|"log"|"cbrt"|"div"|"hypot"|"print"|"rand"|"read"|"equal"|"less"|"greater"


/***************/
/*	  Rules    */
/***************/

%%

"(" {
    fprintf(stderr, "lex: LPAREN\n");
    return LPAREN;
    }

")" {
    fprintf(stderr, "lex: RPAREN\n");
    return RPAREN;
    }

"let" {
    fprintf(stderr, "lex: LET\n");
    return LET;
    }


"int" {
    fprintf(stderr, "lex: INT\n");
    return INT;
    }

"double" {
    fprintf(stderr, "lex: DOUBLE\n");
    return DOUBLE;
    }

"lambda" {
    fprintf(stderr, "lex: LAMBDA\n");
    return LAMBDA;
    }

"cond" {
    fprintf(stderr, "lex: COND\n");
    return COND;
    }

"quit" {
    return QUIT;
    }

{int} {
    yylval.dval = strtod(yytext, NULL);
    fprintf(stderr, "lex: INT dval = %lf\n", yylval.dval);
    return INT_LITERAL;
    }

{double} {
    yylval.dval = strtod(yytext, NULL);
    fprintf(stderr, "lex: DOUBLE dval = %lf\n", yylval.dval);
    return DOUBLE_LITERAL;
    }

{func} {
    yylval.sval = malloc(sizeof(char) * yyleng + 1);
    strcpy(yylval.sval, yytext);
    fprintf(stderr, "lex: FUNC sval = %s\n", yylval.sval);
    return FUNC;
    }

{symbol} {
    yylval.sval = malloc(sizeof(char) + yyleng + 1);
    strcpy(yylval.sval, yytext);
    fprintf(stderr, "lex: SYMBOL sval = %s\n", yylval.sval);
    return SYMBOL;
    }

[\n] {
    fprintf(stderr, "lex: EOL\n");
    YY_FLUSH_BUFFER;
    return EOL;
    }

[ \t] ;

. {
    printf("ERROR: invalid character: >>%s<<\n", yytext);
  }

%%


/**************/
/*  Use Code  */
/**************/

int main(void) {

    freopen("/dev/null", "w", stderr); // except for this line that can be uncommented to throw away debug printouts

    char *s_expr_str = NULL;
    size_t s_expr_str_len = 0;
    YY_BUFFER_STATE buffer;
    while (true) {
        printf("\n> ");
        getline(&s_expr_str, &s_expr_str_len, stdin);
        s_expr_str[s_expr_str_len++] = '\0';
        s_expr_str[s_expr_str_len++] = '\0';
        buffer = yy_scan_buffer(s_expr_str, s_expr_str_len);
        yyparse();
        yy_delete_buffer(buffer);
    }
}