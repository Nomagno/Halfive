# Half-World Eloquent Lisp
#### SPDX identifier: COIL-1.0
#### Copyright Nomagno 2022

It is recommended to use the ".hwelq" extension for HW-Eloquent code files (where applicable).
HW-Eloquent is identified by the MIME type `text/hwelq`

### HW-Eloquent is a minimalistic Lisp dialect to abstract over HWVM programs

#### SYNTAX:
- If statement:
  * `(if bool` -> If `bool` evaluates to an unsigned scalar that is not 0
    * `ontrue` -> Evaluate this
    * `onfalse)` -> Otherwise evaluate this
- Begin Statement: `(begin expr1 expr2 ... exprN)`
  * Evaluates expressions in order. The `begin` statement itself evaluates to whatever the last expression evaluates to
- Lambda statement:
  * `(lambda args body)`
  * `args` -> List of variables provided as input, e.g. literally `(a b c)`, or if a single variable, `a`, or if no variables, `%`
  * `body` -> Code to evaluate when calling the procedure, can contain any syntax, input variables, or global variables
  * Evaluates to an unnamed procedure that takes the number of arguments in the `args` list, and evaluates `body`, substituting them inside as appropiate
  * The variable called `self` represents the procedure itself, so it may be called to perform recursive calls, and it will run separately and perform independent variable substitution.
- Procedure call: `(procedure arg1 arg2 ...)`
  * Call procedure with arguments. The arguments are evaluated in left-to-right order before passing them to the procedure.
- Define variable: `(define variable value)`
  * Make variable name mean a specific value globally.
  * EXAMPLE: `(define myfunc (lambda x (if (eq? x 0) (0) (self (sub x 1)) )))`
    * Defines `myfunc` to mean the calling of `myfunc` as many times as the value of the unsigned scalar `x`, and it evaluates deterministically to the unsigned scalar `0`.

### CORE PROCEDURES:

- PROCEDURE: (CONStruct dotted pair): `(cons A B)` -> `(A . B)`
  * Result is known as a cons cell.
- PROCEDURE: `(car a)`
  * Where `a` is a cons cell, evaluates to the left element of `a`.
- PROCEDURE: `(cdr a)`
  * Where `a` is a cons cell, evaluates to the right element of `a`.
- PROCEDURE: `(nil? a)`
  * Evaluates to 1 if `a` is `%`. Otherwise it evaluates to 0.
- PROCEDURE: `(atom? a)`
  * Evaluates to 1 if `a` is an atom. Otherwise it evaluates to 0.
- PROCEDURE: `(procedure? a)`
  * Evaluates to 1 if `a` is a procedure, otherwise it evaluates to 0.
- PROCEDURE: `(eq? a b)`
  * Evaluates to 1 if `a` and `b` are both atoms, and both are the exact same atom. Otherwise it evaluates to 0.

### OPERATION PROCEDURES:
- RULES:
  * The first six apply the operation, and evaluate to a dotted pair of the form `(VAL . CF)`, where `VAL`is an unsigned scalar representing the result, and `CF` is an unsigned scalar that is one if there was overflow/underflow, and zero if there wasn't.
  * `halt` evaluates to `%`, as it is only useful for its side effects.
- PROCEDURE: (bitwise add): `(add x y)`
  * Where `x` and `y` are unsigned scalars.
- PROCEDURE: (bitwise substract): `(sub x y)`
  * Where `x` and `y` are unsigned scalars.
- PROCEDURE: (bitwise and): `(and x y)`
  * Where `x` and `y` are unsigned scalars.
- PROCEDURE: (bitwise or): `(or x y)`
  * Where `x` and `y` are unsigned scalars.
- PROCEDURE: (bitwise xor): `(xor x y)`
  * Where `x` and `y` are unsigned scalars.
- PROCEDURE: (bitwise rot): `(rot x y)`
  * Where `x` and `y` are unsigned scalars.
- PROCEDURE: (address set): `(set addrpair y)`
  * Where `addrpair` is a cons cell of the form `(high . low)`, for the high and low byte
    of the address to set.
  * Where `y` is either an unsigned scalar, or a cons cell of the form `(high . low)`,
    for the high and low byte of the address to read. Evaluates to the value written to `addrpair`.
- PROCEDURE 17: `(halt)`
  * Terminates the program.

### CLARIFICATIONS
- A list is either `%` or a cons cell where the right hand side is a list.
  Hence, `(a . b)`, `a`, `(a . (b . c) . %)` are not lists, but `(a . ((b . %) . %))`, `(a . %)` are lists.
- `(a b c)` notation stands for the list `(a . (b . (c . %)))`. Hence, `(procedure a b c)` stands for `(procedure . (a . (b . (c . %))))`. `((a b) c)` stands for `((a . (b . %)) . (c . %))`.
- In Eloquent, every piece of data is either a cons cell or an atom. The only three possible types for atoms are unsigned scalar, procedure, or `%`. 
- An atom may be checked for with the procedure `atom?`, `%` with the procedure `nil?`, procedure type with `procedure?`, and equality of two atoms with `eq?`.
- In the above text of the specification, whenever `where X is Y` appears, it is to be interpreted as a statement that `X` MUST be of type `Y`, and if it is not, an ERROR occurs.
- Variables, declared using `define` or as `lambda` arguments, are replaced with their associated value.
  In the case of lambdas, their arguments are replaced with the evaluation of the provided expression in the procedure call.
  For example, `(lambda (myvar) (nand myvar 1))` would have `myvar` in the body replaced with the first argument it is passed,
  so `((lambda (myvar) (nand myvar 1)) 2)` would execute code equivalent to `(nand 2 1)`, which evaluates to the (hexadecimal base) scalar FC.
- Variable names may contain any of `abcdefghijklmnopqrstuvwxyz?`
- Unsigned scalars may contain any of `0123456789ABCDEF`
- To `set` addresses lower than `(01 . 00)` is an ERROR. These lower addresses are reserved for special purposes.
- To `set` an address to itself is considered only a read, and it is hence allowed even if the address is read-only.
- Take into account HWVM allows addresses to be READ-ONLY or UNMAPPED. Writing to the former is an ERROR, doing anything with the latter is an ERROR.
- No statement in this specification applies as soon as an ERROR has occurred. The program may continue functioning after handling the ERROR gracefully, it may stop functioning, or it may continue functioning in a noncompliant state. 
