# Half-World Eloquent Lisp
#### SPDX identifier: COIL-1.0
#### Copyright Nomagno 2022

It is recommended to use the ".hwelq" extension for Eloquent code files (where applicable).
Eloquent is identified by the MIME type `text/hwelq`

### Eloquent is a minimalistic Lisp dialect to abstract over HWVM programs

### SYNTAX:
- If statement: `(if bool onfalse ontrue)`
  * If `bool` evaluates to `%`, evaluate `onfalse`, otherwise evaluate `ontrue`.
- Begin Statement: `(begin expr1 expr2 ... exprN)`
  * The expressions are evaluated in order.
  * Evaluates to the value of the last expression.
- Define statement: `(define var val)`
  * Define a local variable, `var`, to mean a specific value, `val`.
  * It may not be redefined in the current scope.
  * Defining a variable with the same name in deeper scopes will cause the name to be eclipsed.
  * Evaluates to `val`.
- Assign statement: `(assign var val)`
  * Where `var` is a variable that has already been defined, change its value to `val`.
  * It may be reassigned in the current scope.
  * It is possible to reassign it in deeper scopes where the name is not eclipsed.
  * Evaluates to `val`.
- Lambda statement: `(lambda args body)`
  * `args` -> List of local variables provided as arguments. `(a b c)` - takes three arguments. `a` - takes a variable number of arguments, collected into a proper list. `%` - takes no arguments.
  * `body` -> Code to evaluate when calling the procedure, can contain any syntax, statements, or variables accessible from the current scope.
  * Evaluates to an unnamed procedure that takes the number of arguments in the `args` list, and evaluates `body`, substituting them inside as appropiate.
  * The variable called `self` represents the procedure itself, so it may be called to do recursion.
- Procedure call: `(procedure arg1 arg2 ...)`
  * Call procedure with arguments. The arguments are evaluated in left-to-right order before passing them to the procedure. Evaluates to whatever the procedure body evaluates.

### CORE PROCEDURES:
- PROCEDURE: (CONStruct dotted pair): `(cons A B)` -> `(A . B)`
  * Result is known as a cons cell.
- PROCEDURE: `(car a)`
  * Where `a` is a cons cell, evaluates to the left element of `a`.
- PROCEDURE: `(cdr a)`
  * Where `a` is a cons cell, evaluates to the right element of `a`.
- PROCEDURE: `(num? a)`
  * Evaluates to 1 if `a` is an unsigned scalar. Otherwise it evaluates to `%`.
- PROCEDURE: `(nil? a)`
  * Evaluates to 1 if `a` is `%`. Otherwise it evaluates to `%`.
- PROCEDURE: `(proc? a)`
  * Evaluates to 1 if `a` is a procedure, otherwise it evaluates to `%`.
- PROCEDURE: `(atom? a)`
  * Evaluates to 1 if `a` is an atom. Otherwise it evaluates to `%`.
- PROCEDURE: `(eq? a b)`
  * Evaluates to 1 if `a` and `b` are both the exact same procedure. Otherwise it evaluates to `%`.
- PROCEDURE: `(apply proc args)`
  * Calls the procedure `proc` with arguments being each element of the proper list `args`. Evaluates to the result of the procedure.

### OPERATION PROCEDURES:
- The first six procedures evaluate to the result of the operation.
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
- PROCEDURE: (bitwise shift): `(shift x y)`
  * Where `x` and `y` are unsigned scalars.
  * Behaviour corresponds to HWVM shift instruction.
- PROCEDURE: (address set): `(set x y)`
  * Where `x` is a cons cell of the form `(high . low)`, for the high and low byte
    of the address to set.
  * Where `y` is either an unsigned scalar, or a cons cell of the form `(high . low)`,
    for the high and low byte of the address to read.
  * To `set` an address to itself is considered only a read.
  * Evaluates to the value written to `x`.
- PROCEDURE 17: `(halt)`
  * Terminates the program.
  * Evaluates to `%`.

### CLARIFICATIONS
- In Eloquent, every piece of data is either a cons cell or an atom. The only three possible types for atoms are `%`, procedure, or unsigned scalar.
- Variable names may contain any of `abcdefghijklmnopqrstuvwxyz?`
- An implementation may reject variable names with more than 6 characters, as long as it is indicated in the documentation.
- Unsigned scalars may contain any of `0123456789ABCDEF`
- Unsigned scalars have a width of 8 bits.
- A list is either `%` or a cons cell where the right hand side is a list.
  Hence, `(a . b)`, `a`, `(a . (b . c) . %)` are not lists, but `(a . ((b . %) . %))`, `(a . %)` are lists.
- `(a b c)` notation stands for the list `(a . (b . (c . %)))`.
- Note that the entire program and any sub-expressions are, themselves, lists.
