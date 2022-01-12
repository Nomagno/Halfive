# Half-World Virtual Machine Specification

#### SPDX identifier: COIL-1.0
#### Copyright Nomagno 2021

It is recommended to use the ".hwvm" extension for HWVM assembly documents (where applicable).
HWVM assembly is identified by the MIME type `text/hwvm`

It is recommended to use the ".hwbin" extension for HWVM binary executables (where applicable).
HWVM binary drives are identified by the MIME type `application/hwbin`

It is recommended to use the ".hwdrive" extension for HWVM read-only drives (where applicable).
HWVM binary drives are identified by the MIME type `application/hwdrive`

### The Half-World virtual machine
***
The Half-World virtual machine, henceforth referred to HWVM, is a standardized, but not fully defined, execution engine for code. This code may be interpreted directly from the assembly format, executed directly from the binary format, or executed in any other equivalent way.

#### The HWVM Instruction notation
The instruction set for the virtual machine is composed of precisely 16 instructions, whose functioning is documented with the following notation: `inst ARG1 ARG2 ARG3`, where ARGx may be noted as either Vx, Rx, or ID. There are three types of arguments: literals, pointers, and addresses. Literals are enclosed in square brackets `[XXXX]`, pointers in keys `{XXXX}`, and addresses are not `XXXX`. All are always written down in hexadecimal base when not talking about the binary format. Arguments of type Vx take literals, pointers and addresses, those of type Rx take only addresses and pointers, and the ones denoted ID take only literals. POINTERS AND LITERALS SHALL NEVER BE COMBINED IN AN INSTRUCTION'S ARGUMENTS

***
### HWVM Memory layout (Code memory and data memory)
#### IMPORTANT NOTE: ALL MEMORY IN CODE AND DATA MEMBERS HAS TO BE INITIALIZED TO 0 (ZERO)!
Following the legacy Harvard architecture, HWVM loads code and data separately into different 'chips', memory sections, or however it may be implemented. The executable section will be henceforth referred to as 'code member', and the data section will be henceforth referred to as 'data member'.

HWVM has two major data structures: the byte and the integer. Both are raw memory, and bytes must be able to hold the values 0x00 to 0xFF, so on a typical computer they need to be unsigned integer cells of at least 8 bits in size. The machine is also required to handle the code member in cells that can have values from 0x0000 to 0xFFFF, or minimum 16-bits.

It is encouraged to denote two basic memory units trough constants, variables, or identifiers: `HWVM_MEMSIZE`, and `HWVM_MEMSMALL`. `HWVM_MEMSIZE` denotes the basic amount of bytes/integers that may be allocated, and is 0x1000. `HWVM_MEMSMALL` denotes a fraction of `HWVM_MEMSIZE`, meant for more precise assignments, 0x400.

Note: Altough bytes can only hold 8 bits of information, and so literals may only range from 0x00 to 0xFF, the address space of the VM is 16 bits, and the machine must hence be able to read addresses ranging from 0x0000 to 0xFFFF. *Pointers are memory addresses read as the value of two contiguous bytes from memory*

#### There are two structures inside the code member:
The instruction storage can hold 0x4000 bytes/ints, or four times the value of HWVM_MEMSIZE.

- The operand storage can hold at least 0x4000 GROUPS OF FOUR INTEGERS, that is three arguments and a literal/pointer specifier.

- The literal/pointer specifier denotes which of the three first operands are literals, from left to right, in THE LEAST THREE SIGNIFICANT BINARY BITS, AND THEY ARE POINTERS IF THE FOURTH IS ONE: 1111 means all three are arguments are pointers, 0110 means only the first two are literals, 1101 means only the first and third are pointers, 1100 means only the first is a pointer, 0010 means only the second is literal, 1001 means only the third is a pointer.

- Those arguments that are not literals are implicitly addresses.

- Each row (of 4 elements) of the two dimensional operand array corresponds to the same index of the onedimensional instruction array, and each instruction takes a fixed number of arguments. Hence, it is unambiguous which operands have to be read at each index of the operand array.
- Both of these blocks of memory SHALL be filled prior to execution, be it reading from the later specified assembly, the later specified binary format, or some other format.

#### There are three structures inside the data member:
- The general purpose memory holds four times the HWVM_MEMSIZE number of bytes, or 0x4000 (16 KiBs), and SHALL be mapped continously from 0x0000 to 0x3FFF. It may be manipulated by code at will, with no restrictions.

- The DRIVE or ROM memory can hold double the general purpose memory's amount of bytes, 0x8000 (32KiBs), and SHALL be mapped continously from 0x4000 to 0xBFFF.

- THE DRIVE SHOULD BE LOADED AS A RAW BYTESTREAM FROM A FILE ON-DISK, AND IT IS READ-ONLY. WRITING RESULTS IN AN ERROR.

- The range of addresses from 0xC000 to 0xFFE9 is reserved for implementation-defined or future-standard-defined peripherals or other extensions to the VM.

##### Special registers (0xFFF0-0xFFFF):
- The 0xFFF0 address always results in program error, and it can be safely used for this purpose by implementing.

- The 0xFFF1 to 0xFFF9 range is reserved for implementation-defined or future-standard-defined special registers.

- The 0xFFFA register is READ-ONLY (WRITING MIGHT RESULT IN EXECUTION ERROR), and represents the current program counter (Higher 8 bits). It is writable trough the `jmp`, `jcz`, and `jcnz` instructions (specified later in the document).

- The 0xFFFB register is READ-ONLY (WRITING MIGHT RESULT IN EXECUTION ERROR), and represents the current program counter (Lower 8 bits). It is writable trough the `jmp`, `jcz`, and `jcnz` instructions (specified later in the document).

- The 0xFFFC register is WRITE-ONLY (READING MIGHT RESULT IN EXECUTION ERROR), and is the only OUTPUT REGISTER defined by this document. Any value written to it will be displayed to the user/programmer.

- The 0xFFFD register is READ-ONLY (WRITING MIGHT RESULT IN EXECUTION ERROR), and is the only INPUT REGISTER defined by this document. Whenever it is read from by any instruction, it MAY trigger a blocking prompt from input of the user, but it is OPTIONAL. IF there is a blocking prompt, it SHALL pause program execution in such a way the execution environment can not tell it has been paused. IF there is not a blocking prompt, it SHALL update with user input periodically, be it at fixed intervals, at irregular intervals, or whenever there is a change.

- The 0xFFFE register is the CARRY FLAG. It is SET TO ONE (1) if there is UNDERFLOW (when performing SUBSTRACTION with the `sub` or `cmp` instructions), or OVERFLOW (when performing ADDITION with the `add` instruction). If the operation does NOt have any overflow or underflow, it is SET TO ZERO (0). It otherwise SHALL be just as modifiable as the GENERAL PURPOSE MEMORY.

- the 0xFFFF register is the ZERO FLAG. It is SET TO ZERO (0) if the result of an ADDITION, SUBSTRACTION, COMPARISON, BINARY OR, BINARY XOR, or BINARY AND is ZERO. If the operation does NOT result in zero, it is SET TO ONE (1). It otherwise SHALL be just as modifiable as the GENERAL PURPOSE MEMORY. It SHALL be always set IF THE RESULT OF ONE OF THESE OPERATIONS IS ZERO, but ALWAYS BEFORE WRITING TO THE DESTINATION ADDRESS. IF the result of one of this operations is NOT ZERO, it SHALL be set to ONE (1), but ALWAYS BEFORE WRITING TO THE DESTINATION ADDRESS.

Extra note: the program counter is incremented every time an instruction is executed, however the jmp/jcz/jcnz instructions can forcible modify it without increasing it, hence being the only instructions capable of taking 16-bit/integer literals. All instructions in a program SHALL be LINEARLY NUMBERED from ZERO (0) the first to the last, and the program counter SHALL indicate execution of each instruction at any given time.

***
### The instruction set

There are currently SIXTEEN (16) instructions, each numbered with the decimal number in PARENTHESIS `()` for later reference.

- Each instruction takes the number of register, value, or ID arguments specified, and only that number (it is an ERROR to specify MORE or LESS).

- In values Vx, if an address is specified, the instruction SHALL read the value of that address. If a literal is specified, the instruction SHALL perform no address reading and use that literal.

- If a pointer is specified, the instruction shall read the value of the address pointed two by THAT ADDRESS AND THE NEXT ONE (e.g. {2} denotes the contents of memory address 0x02 and 0x03, TREATED AS A SINGLE 16-BIT INTEGER).

- In addresses Rx, literals are an ERROR but pointers are ALLOWED.

- In idenfitications ID, addresses or pointers are an ERROR.

- LITERALS AND POINTERS SHALL NEVER BE SPECIFIED TOGETHER IN THE SAME ARGUMENT LIST

```
halt (0) - TAKES NO ARGUMENTS, STOPS PROGRAM EXECUTION
nop (1) - TAKES NO ARGUMENTS, DOES NOTHING FOR A FULL CYCLE
set (2) V1 R2; SETS ADDRESS R2 *TO* VALUE V1
jmp (3) V1 - JUMP (MOVE THE PROGRAM COUNTER, HAND EXECUTION) *TO* VALUE V1. Can take 16-bit literals
jcz (4) V1 - jmp TO V1 *IF* 0xFFFF IS ZERO (0).  Can take 16-bit literals
add (5) V1 V2 R3 - ADD V1 AND V2, WRITE THE RESULT TO R3. SETS CARRY/ZERO FLAGS APPROPIATELY
sub (6) V1 V2 R3 - SUBSTRACT V2 *FROM* V1, WRITE THE RESULT TO R3. SETS CARRY/ZERO FLAGS APPROPIATELY
and (7) V1 V2 R3 - PERFORM A BINARY 'and' ON V1 AND V2, WRITE THE RESULT TO R3. SETS ZERO FLAG APPROPIATELY
or (8) V1 V2 R3 - PERFORM A BINARY 'or' ON V1 AND V2, WRITE THE RESULT TO R3. SETS ZERO FLAG APPROPIATELY
xor (9) V1 V2 R3 - PERFORM A BINARY 'xor' ON V1 AND V2, WRITE THE RESULT TO R3. SETS ZERO FLAG APPROPIATELY
rot (10) V1 V2 R3 - IF V1 IS 0 THROUGH 7, BITSHIFT V2 LEFT BY V1, WRITE THE RESULT TO R3.
    ELSE IF V1 IS 8 THROUGH F, BITSHIFT V2 RIGHT BY (V1 - 8), WRITE THE RESULT TO R3. ELSE DO NOTHING

cmp (11) V1, V2 - SUBSTACT V2 *FROM* V1, BUT *WITHOUT* SAVING THE RESULT. SETS CARRY/ZERO FLAGS APPROPIATELY
subs (12) ID - SEE SECTION BELOW
sube (13) ID - SEE SECTION BELOW
call (14) ID - SEE SECTION BELOW
jcnz (15) V1 - jmp TO V1 *IF* 0xFFFF is NOT ZERO. Can take 16-bit literals
```

***
### Subroutines

- Subroutines are RECOMMENDED to be defined at the top of program files, so at to avoid any call ever being made without subs having initialized the appropiate memory sections first. It is OPTIONAl, however

- Subroutines are handled with the instructions 'call', 'sube', 'subs'. Each subroutine instruction has an `[ID]` argument, to identify the subroutines. DECLARING TWO SUBROUTINES WITH THE SAME IDENFITICATION IS AN ERROR.

- A subs instruction (subroutine START) ALWAYS has to be matched with a sube instruction (subroutine END). They can have any number of instruction inbetween, as long as no `call`s to ITS OWN ID (RECURSION) are ever contained within, and NO NEW SUBROUTINES ARE STARTED/ENDED within it.
- The subs instruction SHALL write to an OUT-OF-MEMORY (not mapped to the address space) location the program counter of the instruction RIGHT AFTER its corresponding sube (referre to as SKIPCOUNT), corresponding to the ID argument index, as to skip the rest of the subroutine when encountered normally. After that, it SHALL write to an OUT-OF-MEMORY location the program counter RIGHT AFTER itself (referred to as EXECOUNT), corresponding to the ID argument, so as to allow call instructions to enter execution.

- The call instruction SHALL write the program counter of the instruction RIGHT AFTER itself to an OUT-OF-MEMORY location, (RETURNCOUNT) corresponding to the ID argument index. After that, it SHALL set the program counter to EXECOUNT

- The sube instruction SHALL set the program counter to the corresponding RETURNCOUNT of the ID argument.

***
### Assembly language
#### The assembly language is the exact representation of instructions, literals, pointers and addresses that has been discussed so far (comments are not part of the language, but they are designated by everything after a `#` here, for clarity porpuses. The line numbering is not part of the language, either, it's just to make keeping track of the program counter easier):
```
0. set [33] FFFC # Write literal 0x33 (51, 00110011) to the OUTPUT REGISTER
1. jmp 4 # Jump to instruction 4
2. cmp [4] 3 # Do the subtraction [4] - 3. We don't know what address three contains,
   but it will set the zero flag if it is 0x4
3. jcz 7 # If the previous comparison was indeed correct and address 3 contains 0x4,
   jump to instruction 7
4. add [FE] [8] FFFC # Write the addition of 0xFE (254, 11111110) and 0x8 (8, 00001000)
   to the OUTPUT register. It should result in OVERFLOW
5. set FFFE FFFF # Copy the carry flag to the zero flag
6. jcnz 8 # If the zero flag is not zero (it shouldn't be, there was overflow and
   the carry flag was cloned!), jump to instruction 8
7. halt
8. jmp 2 # Jump to instruction 2
9. halt
```

***
### Binary format
A specification of the format follows, it still applies:
```
BINARY FORMAT:
You code each instruction as 4 bits of which the least significant 3 indicate
which arguments are literals and which addresses, and the fourth indicates if the
literals are pointers or not (0101 - from right to left:  third is literal,
second address, first literal, fourth indicates they are literals and NOT pointers),
then 4 bits for the instructions themselves (0-15), then
simply read the corresponding number of 16-bit 'arguments' (0-3), and repeat.

EXAMPLE:

BINARY:    0010 0101 0000000000000001 0000000000000011 0000000000000010 0000 0000000000000000
DECIMAL:   2    5    1                3                2                0    0
ASSEMBLY:       add  1               [3]               2               \n    halt
ENGLISH:
	The first instruction has the middle argument as a literal.
	The first instruction is 'add'.
	ADD the contents of Address 1 to the number 3, put the result into Address 2.

	The second instruction has no arguments as literals.
	The second instruction is 'halt'.
	HALT.
```

***
### SUMMARY OF DATA STORAGE TYPES
You can store code as either .hwvm ASSEMBLY LANGUAGE files, or .hwbin BINARY FILES, and can store data as .hwdrive READ-ONLY MEMORY files.
