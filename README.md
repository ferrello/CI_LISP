# Channel Islands Lisp
Implement a compiler using flex (lex) and bison (yacc). The implementation will span several stages; each stage will add a feature to the interpreter. The later stages are much more challenging than the initial stages.

:exclamation: Only completed to Task 7 !

## Author
> Ferid Ruano

> CSU Channel Islands

> COMP 232 - Programming Languages


## Tasks
### Task 1 - Math

We will be constructing and evaluating abstract syntax trees representing the Cambridge Polish Notation input.

:exclamation: Results of DOUBLE_TYPE that are whole numbers will be converted to INT_TYPE to remove trailing zeros.

`> (add 1 2)`
`> (mult 2.5 3)`
`> (pow 16 3.5)`
`> (hypot 3 4)`

### Task 2 - Variables

Extends the Bison (yacc) grammar to accommodate variables that in Lisp jargon are called symbols. Variables exist in potentially nested scopes that can be defined by the let section construct preceding CILisp s-expressions. Each symbol assumes a value represented by the associated s-expression. A symbol can be any number of small and capital letters.

:exclamation: Results of DOUBLE_TYPE that are whole numbers will be converted to INT_TYPE to remove trailing zeros.

`> ((let (x 1))(neg x))`

`> (add ((let (abc 1)) (sub 3 abc)) 4)`

`> ((let (abc 1)) (sub ((let (abc 2) (de 3)) (add abc de)) abc))`

### Task 3 - Type Casting

Extend the capabilities of ciLisp by adding type-casting. The types apply to numbers and symbol definitions. The type of a symbol may (optionally) be assigned in the extended let section that now allows specifying types of symbols. Similarly, the type of a number literal may be assigned. The type of more complicated s-expressions has to be derived during evaluation of the expression from the types of its elements.

`> (mult ((let (double a 1) (int b 2)) (add a b)) (sqrt 2))`

`> (add ((let (int a ((let (double b 2)) (mult b (sqrt 10))))) (div a 2)) ((let (c 5)) (sqrt c)))`

### Task 4 - Print Expression

Extend the capabilities of ciLisp by adding a unary print function. The print function should display the value of the s-expression in the format depending on the type of the expression. Printing should happen when the f_expr is evaluated, NOT when the function ast node is created. When said f_expr is evaluated, the returned RET_VAL should be the result of evaluating the printed s_expr.

`> (print s_expr)`

### Task 5 - Parameter Lists

 Expands the capability of ciLisp by adding support for parameter lists of arbitrary length. The number of parameters in the list of parameters must meet the needs of the function. If there are too few parameters provided, the evaluator should issue a runtime error:
 `Error :: <function> :: Too few parameters for the function` and return the value of NAN. If more than necessary parameters are provided for a given function, then the evaluator should use as many as appropriate, and issue a runtime warning:
 `Warning :: <function> :: Too many parameters for the function`.
 
 The operations of addition and multiplication should accommodate any number of parameters. Print should print the values of all s-expressions in the list in one line separated by spaces and return the value of the last s-expression in the list as its own value.
 
 ```
> ((let (int a 1)(double b 2))(print a b 3))
  => 1 2.000000 3 
  RET_VAL :: INT_TYPE :: 3
```

### Task 6 - Conditional Expressions

Expands the capability of ciLisp by adding conditional expressions and capability to read user provided and random values
Add read as a parameter-less function that takes values from the user. The user should be prompted to enter a value by displaying a prompt:

```
> ((let (int a (read)) (double b (read)) (c (read)) (d (read))) (print a b c d))
Read := 5
Read := 5.5
Read := 6
Read := 6.5
=> 5 5.500000 6 6.500000 
RET_VAL :: DOUBLE_TYPE :: 6.500000
```

Add rand as a function that generates pseudo-random numbers. The function does not take any parameters, and generates a random double 
value between 0 and 1. Note that the C function rand creates a random integer from 0 to RAND_MAX, so typecasting and division by RAND_MAX 
with this function can be used to create a random double in the range 0 to 1. It can be called as follows:

`((let (a 0)) (cond (less (rand) 0.5) (add a 1) (sub a 1)))`

Add comparison functions equal, less, and greater.  The functions return 1 if the condition holds, and 0 otherwise. So (less (rand) 0.5) returns 1 if the randomly generated number is less than 0.5, and returns 0 otherwise.

Added cond function that checks if the first s-expression is true (non-zero). If so, then the next expression is evaluated and returned. Otherwise, the third s-expression is evaluated and returned.

`((let (myA (read))(myB (rand)))(cond (less myA myB) (print myA) (print myB)))`

### Task 7 - User Defined Function

Expands the capability of ciLisp by adding support for user-defined functions. Function definitions should be placed in the let section. A function should have a positive number of formal arguments (symbols). The body of the function is an expression that may use formal parameters. 
Similarly to the use of let and cond, the lambda keyword should be used just for parsing.

`((let (double myFunc lambda (x y) (mult (add x 5) (sub y 2)))) (sub (myFunc 3 5) 2))`

`((let (f lambda (x y) (add x y)))(f (sub 5 2) (mult 2 3))) `

### Task 8 - Recursive User Defined Function

:exclamation: Did not complete!