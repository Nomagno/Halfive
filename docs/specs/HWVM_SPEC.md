# Half-World Virtual Machine Specification

#### SPDX identifier: COIL-1.0
#### Copyright Nomagno 2021, 2022

It is recommended to use the ".hwvm" extension for HWVM assembly documents (where applicable).
HWVM assembly is identified by the MIME type `text/hwasm`

It is recommended to use the ".hwbin" extension for HWVM binary executables (where applicable).
HWVM binary drives are identified by the MIME type `application/hwbin`

It is recommended to use the ".hwdrive" extension for HWVM read-only drives (where applicable).
HWVM binary drives are identified by the MIME type `application/hwdrive`

### The Half-World virtual machine
***
The Half-World virtual machine, henceforth referred to HWVM, is a standardized, but not fully defined, execution engine for code. This code may be interpreted directly from the assembly format, executed directly from the binary format, or executed in any other equivalent way.

#### The HWVM Instruction notation
The instruction set for the virtual machine is composed of precisely 16 instructions, whose functioning is documented with the following notation: `inst ARG1 ARG2`, where ARGx may be noted as either Vx, Rx, or ID. There are three types of arguments: literals, pointers, and addresses. Literals are prefixed by equal `=XXXX`, pointers by and `&XXXX`, and addresses are not `XXXX`. All are always written down in hexadecimal base when not talking about the binary format. Arguments of type Vx take literals, pointers and addresses, those of type Rx take only addresses and pointers, and the ones denoted ID take only literals. POINTERS AND LITERALS SHALL NEVER BE COMBINED IN AN INSTRUCTION'S ARGUMENTS

***
### HWVM Memory layout (Code memory and data memory)
#### IMPORTANT NOTE: ALL MEMORY IN CODE AND DATA MEMBERS HAS TO BE INITIALIZED TO 0 (ZERO)!
Following the legacy Harvard architecture, HWVM loads code and data separately into different 'chips', memory sections, or however it may be implemented. The executable section will be henceforth referred to as 'code member', and the data section will be henceforth referred to as 'data member'.

HWVM has two major data structures: the byte and the integer. Both are raw memory, and bytes must be able to hold the values 0x00 to 0xFF, so on a typical computer they need to be unsigned integer cells of at least 8 bits in size. The machine is also required to handle the code member in cells that can have values from 0x0000 to 0xFFFF, or minimum 16-bits.

It is encouraged to denote two basic memory units trough constants, variables, or identifiers: `HWVM_MEMSIZE`, and `HWVM_MEMSMALL`. `HWVM_MEMSIZE` denotes the basic amount of bytes/integers that may be allocated, and is 0x1000. `HWVM_MEMSMALL` denotes a fraction of `HWVM_MEMSIZE`, meant for more precise assignments, 0x400.

Note: Altough bytes can only hold 8 bits of information, and so literals may only range from 0x00 to 0xFF, the address space of the VM is 16 bits, and the machine must hence be able to read addresses ranging from 0x0000 to 0xFFFF. *Pointers are memory addresses read as the value of two contiguous bytes from memory*

#### There are two structures inside the code member:
The instruction storage can hold 0x4000 bytes/ints, or four times the value of HWVM_MEMSIZE.

- The operand storage can hold at least 0x4000 GROUPS OF THREE INTEGERS, that is two operands and a literal/pointer specifier.

- The literal/pointer specifier denotes which of the two operands are literals, from left to right, in THE TWO LEAST SIGNIFICANT BINARY BITS, AND THEY ARE POINTERS IF THE FOURTH IS ONE: 1011 means both operands are pointers, 0010 means only the first is a literal, 1001 means only the second is a pointers, 1010 means only the first is a pointer, 0010 means only the first is literal, 1001 means only the second is a pointer. So, in NXNN, bit X goes unused

- Those arguments that are not literals are implicitly addresses.

- Each row (of 3 elements) of the two dimensional operand array corresponds to the same index of the onedimensional instruction array, and each instruction takes a fixed number of arguments. Hence, it is unambiguous which operands have to be read at each index of the operand array.
- Both of these blocks of memory SHALL be filled prior to execution, be it reading from the later specified assembly, the later specified binary format, or some other format.

#### There are three structures inside the data member:
- The general purpose memory holds four times the HWVM_MEMSIZE number of bytes, or 0x4000 (16 KiBs), and SHALL be mapped continously from 0x0000 to 0x3FFF. It may be manipulated by code at will, with no restrictions.

- The DRIVE or ROM memory can hold double the general purpose memory's amount of bytes, 0x8000 (32KiBs), and SHALL be mapped continously from 0x4000 to 0xBFFF.

- THE DRIVE SHOULD BE LOADED AS A RAW BYTESTREAM FROM A FILE ON-DISK, AND IT IS READ-ONLY. WRITING RESULTS IN AN ERROR.

- The range of addresses from 0xC000 to 0xFFE9 is reserved for implementation-defined or future-standard-defined peripherals or other extensions to the VM.

##### Special registers (0xFFF0-0xFFFF):
- The 0xFFF0 address always results in program error, and it can be safely used for this purpose by implementing.

- The 0xFFF1 to 0xFFF9 range is reserved for implementation-defined or future-standard-defined special registers.

- The 0xFFFA register is READ-ONLY (WRITING MIGHT RESULT IN EXECUTION ERROR), and represents the current program counter (Higher 8 bits). It is writable trough the `jmp`, `skpz`, and `skmz` instructions (specified later in the document).

- The 0xFFFB register is READ-ONLY (WRITING MIGHT RESULT IN EXECUTION ERROR), and represents the current program counter (Lower 8 bits). It is writable trough the `jmp`, `skpz`, and `skmz` instructions (specified later in the document).

- The 0xFFFC register is WRITE-ONLY (READING MIGHT RESULT IN EXECUTION ERROR), and is the only OUTPUT REGISTER defined by this document. Any value written to it will be displayed to the user/programmer.

- The 0xFFFD register is READ-ONLY (WRITING MIGHT RESULT IN EXECUTION ERROR), and is the only INPUT REGISTER defined by this document. Whenever it is read from by any instruction, it MAY trigger a blocking prompt from input of the user, but it is OPTIONAL. IF there is a blocking prompt, it SHALL pause program execution in such a way the execution environment can not tell it has been paused. IF there is not a blocking prompt, it SHALL update with user input periodically, be it at fixed intervals, at irregular intervals, or whenever there is a change.

- The 0xFFFE register is the CARRY FLAG. It is SET TO ONE (1) if there is UNDERFLOW (when performing SUBSTRACTION with the `sub` or `cmp` instructions), or OVERFLOW (when performing ADDITION with the `add` instruction). If the operation does NOt have any overflow or underflow, it is SET TO ZERO (0). It otherwise SHALL be just as modifiable as the GENERAL PURPOSE MEMORY.

- the 0xFFFF register is the ZERO FLAG. It is SET TO ZERO (0) if the result of an ADDITION, SUBSTRACTION, COMPARISON, BINARY OR, BINARY XOR, or BINARY AND is ZERO. If the operation does NOT result in zero, it is SET TO ONE (1). It otherwise SHALL be just as modifiable as the GENERAL PURPOSE MEMORY. It SHALL be always set IF THE RESULT OF ONE OF THESE OPERATIONS IS ZERO, but ALWAYS BEFORE WRITING TO THE DESTINATION ADDRESS. IF the result of one of this operations is NOT ZERO, it SHALL be set to ONE (1), but ALWAYS BEFORE WRITING TO THE DESTINATION ADDRESS.

Extra note: the program counter is incremented every time an instruction is executed, however the jmp/skpz/skmz instructions can forcible modify it without increasing it, hence being the only instructions capable of taking 16-bit literals. All instructions in a program SHALL be LINEARLY NUMBERED from ZERO (0) the first to the last, and the program counter SHALL indicate execution of each instruction at any given time.

***
### The instruction set

There are currently SIXTEEN (16) instructions, each numbered with the decimal number in PARENTHESIS `()` for later reference.

- Each instruction takes the number of register, value, or ID arguments specified, and only that number (it is an ERROR to specify MORE or LESS).

- In operands marked value Vx, if an address is specified, the instruction SHALL read the value of that address. If a literal is specified, the instruction SHALL perform no address reading and use that literal.

- If a pointer is specified, the instruction shall read the value of the address pointed two by THAT ADDRESS AND THE NEXT ONE (e.g. &2 denotes the contents of memory address 0x02 and 0x03, TREATED AS A SINGLE 16-BIT INTEGER).

- In operands marked register Rx, literals are an ERROR, addresses are ALLOWED, pointers are ALLOWED.

- In idenfitications ID, addresses or pointers are an ERROR.

- LITERALS AND POINTERS SHALL NEVER BE SPECIFIED TOGETHER IN THE SAME OPERAND LIST

```
halt (0) - TAKES NO ARGUMENTS, STOPS PROGRAM EXECUTION
nop (1) - TAKES NO ARGUMENTS, DOES NOTHING (*RESERVED FOR FUTURE USAGE*).
jmp (2) V1 - JUMP (MOVE THE PROGRAM COUNTER, HAND EXECUTION) *TO* VALUE V1. SPECIAL EXCEPTION: Take 16-bit literals,
    addresses are treated as 16-bit literals, and pointers are treated as addresses with a 16-bit value
skpz (3) - LITERAL; add (LITERAL+1) to PROGRAM COUNTER *IF* 0xFFFF is ZERO (0), where LITERAL is a literal.
skmz (4) - LITERAL; substract (LITERAL-1) from PROGRAM COUNTER *IF* 0xFFFF is ZERO (0), where LITERAL is a literal.

set (5) R1 V2; SETS ADDRESS R1 *TO* VALUE V2
add (6) R1 V2  - ADD R1 AND V2, WRITE THE RESULT TO R1. SETS CARRY/ZERO FLAGS APPROPIATELY
sub (7) R1 V2  - SUBSTRACT V2 *FROM* R1, WRITE THE RESULT TO R1. SETS CARRY/ZERO FLAGS APPROPIATELY
and (8) R1 V2  - PERFORM A BINARY 'and' ON R1 AND V2, WRITE THE RESULT TO R1. SETS ZERO FLAG APPROPIATELY
or (9) R1 V2 - PERFORM A BINARY 'or' ON R1 AND V2, WRITE THE RESULT TO R1. SETS ZERO FLAG APPROPIATELY
xor (10) R1 V2 - PERFORM A BINARY 'xor' ON R1 AND V2, WRITE THE RESULT TO R1. SETS ZERO FLAG APPROPIATELY
shift (11) R1 V2 - IF V2 IS 0 THROUGH 7, BITSHIFT R1 LEFT BY V2, WRITE THE RESULT TO R1.
    ELSE IF V2 IS 8 THROUGH F, BITSHIFT R1 RIGHT BY (V2 - 8), WRITE THE RESULT TO R1. ELSE DO NOTHING

cmp (12) V1, V2 - SUBSTACT V2 *FROM* V1, BUT *WITHOUT* SAVING THE RESULT. SETS CARRY/ZERO FLAGS APPROPIATELY
func (13) ID - SEE SECTION BELOW
ret (14) ID - SEE SECTION BELOW
call (15) ID - SEE SECTION BELOW
```

***
### Subroutines

- Subroutines are RECOMMENDED to be defined at the top of program files, so at to avoid any call ever being made without FUNC having initialized the appropiate memory sections first. It is OPTIONAl, however.

- Subroutines are handled with the instructions 'CALL', 'RET', 'FUNC'. Each subroutine instruction has an `=ID` argument, to identify the subroutines. DECLARING TWO SUBROUTINES WITH THE SAME IDENFITICATION IS AN ERROR.

- A FUNC instruction (subroutine START) ALWAYS has to be matched with a `ret` instruction (subroutine END). They can have any number of instruction inbetween, as long as no `call`s to ITS OWN ID (RECURSION) are ever contained within, and NO NEW SUBROUTINES ARE STARTED/ENDED within it.
- The FUNC instruction SHALL write to an OUT-OF-MEMORY (not mapped to the address space) location the program counter of the instruction RIGHT AFTER its corresponding `ret` (referre to as SKIPCOUNT), corresponding to the ID argument index, as to skip the rest of the subroutine when encountered normally. After that, it SHALL write to an OUT-OF-MEMORY location the program counter RIGHT AFTER itself (referred to as EXECOUNT), corresponding to the ID argument, so as to allow call instructions to enter execution.

- The CALL instruction SHALL write the program counter of the instruction RIGHT AFTER itself to an OUT-OF-MEMORY location, (RETURNCOUNT) corresponding to the ID argument index. After that, it SHALL set the program counter to EXECOUNT

- The ret instruction SHALL set the program counter to the corresponding RETURNCOUNT of the ID argument.

***
### Assembly language
#### The assembly language is the exact representation of instructions, literals, pointers and addresses that has been discussed so far.

***
### Binary format
A specification of the format follows:
```
BINARY FORMAT:
You code each instruction as 4 bits of which the least significant 2 indicate
which arguments are literals and which addresses, and the highest indicates if
the literals are pointers or not (0001 - from right to left:  first is literal,
second address, third bit is padding, high bit indicates they are literals and NOT
pointers), then 4 bits for the instructions themselves (check the listed values)
then two 16-bit 'arguments' (0-2),
and repeat.

EXAMPLE:
BINARY: 0001 0101 000000000000001 000000000011111 0000 0000 0000000000000000 0000000000000000
DECIMAL:   1    5               1              31 0       0                0                0
ASSEMBLY:     add               1             =1F      halt
ENGLISH: add the contents of address ONE and the number 1F, put the result back into address ONE, halt
```

***
### SUMMARY OF DATA STORAGE TYPES
You can store code as either .hwvm ASSEMBLY LANGUAGE files, or .hwbin BINARY FILES, and can store data as .hwdrive READ-ONLY MEMORY files.
