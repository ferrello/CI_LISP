# Channel Islands Lisp
Implement a compiler using flex (lex) and bison (yacc). The implementation will span several stages; each stage will add a feature to the interpreter. The later stages are much more challenging than the initial stages.

## Author
> Ferid Ruano

> CSU Channel Islands

> COMP 232 - Programming Languages


## Tasks
**Task 1 - 11/9/19**

We will be constructing and evaluating abstract syntax trees representing the Cambridge Polish Notation input.

`> (add 1 2)`
`> (mult 2.5 3)`
`> (pow 16 3.5)`
`> (hypot 3 4)`

**Task 2 - 11/9/19**

Extends the Bison (yacc) grammar to accommodate variables that in Lisp jargon are called symbols. Variables exist in potentially nested scopes that can be defined by the let section construct preceding CILisp s-expressions. Each symbol assumes a value represented by the associated s-expression. A symbol can be any number of small and capital letters.

`((let (x 1))(neg x))`
`(add ((let (abc 1)) (sub 3 abc)) 4)`

`((let (abc 1)) (sub ((let (abc 2) (de 3)) (add abc de)) abc))`
