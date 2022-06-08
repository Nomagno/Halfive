# Half-World Eloquent Scheme
#### SPDX identifier: COIL-1.0
#### Copyright Nomagno 2022

It is recommended to use the ".hwelq" extension for HW-Eloquent code files (where applicable).
HW-Eloquent is identified by the MIME type `text/hwelq`

### HW-Eloquent is a minimalistic Scheme Lisp dialect to abstract over HWVM programs

#### SYNTAX:
- N -> evaluates to N, where N is an 8-bit hexadecimal unsigned scalar
- `'(expression)` -> return expression without evaluating, equivalent to `(quote (expression))`:
  * EXAMPLE 1: `((lambda (X) (car (x))) '(cons 1 2))` -> `(car '(cons . (1 . 2)))` -> `'cons`
  * EXAMPLE 2: `((lambda (X) (car (x))) (cons 1 2))` -> `(car (1 . 2))` -> `1`
  * SPECIAL CASE: `'()` -> special value pronounced NIL
- If statement:
  * `(if (bool)` -> If this evaluates to an atom that is not 0
  * `(ontrue)`  -> evaluate this
  * `(onfalse))` -> otherwise evaluate this

- `(procedure arg1 arg2...)` -> call procedure with arguments
- `(begin (expr1) (expr2) (expr3) ... (exprN))` -> Evaluates input expressions in order. The `begin` statement 
                                                   itself evaluates to whatever the last expression evaluates to

### CORE PROCEDURES:

- PROCEDURE 1 (CONStruct dotted pair): `(cons A B)` -> `(A . B)`
- PROCEDURE 2: `(car a)`    -> Left element of dotted pair `a`
- PROCEDURE 3: `(cdr a)`    -> Right element of dotted pair `a`
- PROCEDURE 4: (create an unnamed procedure): 
  * `(lambda (args) (body))`
  * `args` -> list of symbols provided as input, e.g. `(a b c)`
  * `body` -> code to evaluate, can contain any syntax
- PROCEDURE 5: `(bind symbol (args) (body))` -> make symbol mean a specific lambda, can not be redefined:
  * EXAMPLE: `(bind myfunc (x) (fn x))` -> creates procedure that is passed an argument,
  and evaluates to `fn` applied to that argument. can be called like `(myfunc 42)`
- PROCEDURE 6: `(isnil? a)` -> evaluates to 1 if `a` is `'()`, pronounced NIL. Otherwise it evaluates to 0.
- PROCEDURE 7: `(isatom? a)` -> evaluates to 1 if `a` is an atom. Otherwise it evaluates to 0.
- PROCEDURE 8: `(iseq? a b)` -> evaluates to 1 if `a` and `b` are identical, otherwise it evaluates to 0.

### OPERATION PROCEDURES:
- RULES:
  * The first six apply the operation, and evaluate to an unsigned scalar dotted pair of the form `(A . (B . C))`, where `A` is the result, `B` is only zero 
  if the result was zero,  and `C` is only zero wether there was underflow/overflow (only for `add`, `substract`, otherwise its value is undefined)
  * The last two unconditionally evaluate to 0, as they are only useful for their side effects
- PROCEDURE 9: (bitwise add): `(add x y)` -> where `x` and `y` evaluate to unsigned scalars
- PROCEDURE 10: (bitwise substract): `(sub x y)` -> where `x` and `y` evaluate to unsigned scalars
- PROCEDURE 11: (bitwise and): `(and x y)` -> where `x` and `y` evaluate to unsigned scalars
- PROCEDURE 12: (bitwise or): `(or x y)` -> where `x` and `y` evaluate to unsigned scalars
- PROCEDURE 13: (bitwise xor): `(xor x y)` -> where `x` and `y` evaluate to unsigned scalars
- PROCEDURE 14: (bitwise rot): `(xor x y)` -> where `x` and `y` evaluate to unsigned scalars
- PROCEDURE 15: (address set): `(set addrpair y)` -> where `addrpair` is a dotted pair of the form `(high . low)`, 
  for the high and low byte of the address to set, and `y` either evaluates to an unsigned scalar, or is a dotted pair of the form `high . low`,
  for the high and low byte of the address to set
- PROCEDURE 16: `(halt)` -> terminates the program

### SPECIAL RULES
- `'()`, pronounced NIL, is a special value that is treated as 0 when given as input to the operation procedures.
- Lists are dotted pairs that contain as their last element `'()`, pronounced NIL, and where that is the only NIL in the list.
- Atoms, also known as unsigned scalars, are single element, and inputting them alone to `car` or to `cdr` is an ERROR.
- Dotted pairs are the main component of lisp, and every program shall be enclosed in parentheses to mark them. 
  They are essentially multiple scalars or NILs arranged into a binary tree, and inputting them to the operation procedures (except for `set`) is an ERROR.
- Symbols, declared using bind or as lambda arguments, evaluate to their associated value. Quotes symbols `'(symbol)` evaluate to 
  themselves, but if they don't have an associated value the moment they need to be read for any operation, an ERROR results. 
  In the case of lambdas, their arguments are replaced with the evaluation of the provided expression in the procedure call.
  For example, `(lambda (mysymbol) (nand mysymbol 1))` would have `mysymbol` in the body replaced with the first argument it is passed,
  so `(lambda (mysymbol) (nand mysymbol 1) 2)` would have the body evaluate to `(nand 2 1)`, which evaluates to the (hexadecimal base) scalar FC.
- Symbols may contain any of `abcdefghijklmnopqrstuvwxyz?`
- Unsigned scalars may contain any of `0123456789ABCDEF`
- To `set` addresses lower than `(01 . 00)` is an ERROR. These lower addresses are reserved for special purposes.
- Take into account HWVM allows addresses to be READ-ONLY or UNMAPPED. Writing to the former is an ERROR, doing anything with the latter is an ERROR.
