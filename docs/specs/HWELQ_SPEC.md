# Half-World Eloquent Scheme
#### SPDX identifier: COIL-1.0
#### Copyright Nomagno 2022

It is recommended to use the ".hwelq" extension for HW-Eloquent code files (where applicable).
HW-Eloquent is identified by the MIME type `text/hwelq`

### HW-Eloquent is a minimalistic Scheme Lisp dialect to abstract over HWVM programs

#### SYNTAX:
- N -> Evaluates to N, where N is an 8-bit hexadecimal unsigned scalar
- `'(expression)` -> Return expression without evaluating, equivalent to `(quote (expression))`:
  * EXAMPLE 1: `((lambda (X) (car (x))) '(cons 1 2))` -> `(car '(cons . (1 . 2)))` -> `'cons`
  * EXAMPLE 2: `((lambda (X) (car (x))) (cons 1 2))` -> `(car (1 . 2))` -> `1`
  * SPECIAL CASE: `()` -> Special value pronounced NIL
- If statement:
  * `(if (bool)` -> If this evaluates to an atom that is not 0
  * `(ontrue)`  -> Evaluate this
  * `(onfalse))` -> Otherwise evaluate this

- `(procedure arg1 arg2...)` -> Call procedure with arguments. `procedure` must
- `(begin (expr1) (expr2) (expr3) ... (exprN))` -> Evaluates expressions in order. The `begin` statement
                                                   itself evaluates to whatever the last expression evaluates to

### CORE PROCEDURES:

- PROCEDURE 1 (CONStruct dotted pair): `(cons A B)` -> `(A . B)` -> Known as a cons cell
- PROCEDURE 2: `(car a)`    -> Left element of cons cell `a`
- PROCEDURE 3: `(cdr a)`    -> Right element of cons cell `a`
- PROCEDURE 4: (Create an unnamed procedure):
  * `(lambda (args) (body))`
  * `args` -> List of variables provided as input, e.g. `(a b c)`
  * `body` -> Code to evaluate, can contain any syntax
- PROCEDURE 5: `(bind variable (args) (body))` -> Make variable name mean a specific lambda, can not be redefined:
  * EXAMPLE: `(bind myfunc (x) (fn x))` -> Creates procedure that is passed an argument,
  and evaluates to `fn` applied to that argument. can be called like `(myfunc 42)`.
- PROCEDURE 6: `(nil? a)` -> Evaluates to 1 if `a` is `()`, pronounced NIL. Otherwise it evaluates to 0.
- PROCEDURE 7: `(atom? a)` -> Evaluates to 1 if `a` is an atom. Otherwise it evaluates to 0.
- PROCEDURE 8: `(procedure? a)` -> Evaluates to 1 if `a` is a core procedure, otherwise it evaluates to 0.
- PROCEDURE 9: `(eq? a b)` -> Evaluates to 1 if `a` and `b` are identical, otherwise it evaluates to 0.

### OPERATION PROCEDURES:
- RULES:
  * The first six apply the operation, and evaluate to an unsigned scalar cons cell of the form `(A . (B . C))`, where `A` is the result, `B` is only zero
  if the result was zero,  and `C` is only zero wether there was underflow/overflow (only for `add`, `substract`, otherwise its value is undefined).
  * `halt` evaluates to 0, as it is only useful for its side effects.
- PROCEDURE 10: (bitwise add): `(add x y)` -> Where `x` and `y` evaluate to unsigned scalars.
- PROCEDURE 11: (bitwise substract): `(sub x y)` -> where `x` and `y` evaluate to unsigned scalars.
- PROCEDURE 12: (bitwise and): `(and x y)` -> Where `x` and `y` evaluate to unsigned scalars.
- PROCEDURE 13: (bitwise or): `(or x y)` -> Where `x` and `y` evaluate to unsigned scalars.
- PROCEDURE 14: (bitwise xor): `(xor x y)` -> Where `x` and `y` evaluate to unsigned scalars.
- PROCEDURE 15: (bitwise rot): `(rot x y)` -> Where `x` and `y` evaluate to unsigned scalars.
- PROCEDURE 16: (address set): `(set addrpair y)` -> Where `addrpair` is a cons cell of the form `(high . low)`,
  for the high and low byte of the address to set, and `y` either evaluates to an unsigned scalar, or is a cons cell of the form `(high . low)`,
  for the high and low byte of the address to read. Evaluates to the value written to `addrpair`.
- PROCEDURE 17: `(halt)` -> Terminates the program.

### SPECIAL RULES
- Lists are cons cells where the right hand side is either a list or `()`.
  Hence, `(a . b)`, `a`, `(a . (b . c) . ())` are not lists, but `(a . (b . ()) . ())`, `(a . ())` are lists.
- `(a b c)` notation stands for the list `(a . (b . (c . ())))`. Hence, `(procedure a b c)` stands for `(procedure . (a . (b . (c . ()))))`. `((a b) c)` stands for `((a . (b . ())) . (c . ()))`.
- In Eloquent, every piece of data is either a cons cell or an atom. The only three possible types for atoms are unsigned scalar, core procedure, or `()`. An atom may be checked for with the procedure `atom?`, `()` with the procedure `nil?`, core procedure type with `procedure?`, and equality of two pieces of data with `eq?`. Giving atoms as arguments to `car` or to `cdr` is an ERROR.
- Variables, declared using bind or as lambda arguments, are replaced with their associated value. Variables may not appear in quoted expressions, since they don't mean anything there.
  In the case of lambdas, their arguments are replaced with the evaluation of the provided expression in the procedure call.
  For example, `(lambda (myvar) (nand myvar 1))` would have `myvar` in the body replaced with the first argument it is passed,
  so `((lambda (myvar) (nand myvar 1)) 2)` would have the body evaluate to `(nand 2 1)`, which evaluates to the (hexadecimal base) scalar FC.
- Variable names may contain any of `abcdefghijklmnopqrstuvwxyz?`
- Unsigned scalars may contain any of `0123456789ABCDEF`
- To `set` addresses lower than `(01 . 00)` is an ERROR. These lower addresses are reserved for special purposes.
- To `set` an address to itself is considered only a read, and it is hence allowed even if the address is read-only.
- Take into account HWVM allows addresses to be READ-ONLY or UNMAPPED. Writing to the former is an ERROR, doing anything with the latter is an ERROR.
