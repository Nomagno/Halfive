# Halfive Virtual Machine Specification

#### SPDX identifier: COIL-1.0
#### Copyright Nomagno 2021, 2022, 2023

H5VM assembly is identified by the MIME type `text/x-h5asm`, extension ".h5asm"

H5VM machine code sequences or "executables" are identified by the MIME type `application/x-h5bin`, extension ".h5bin"

H5VM drives are identified by the MIME type `application/x-h5drive`, extension ".h5drive"

### The Halfive virtual machine
The Halfive virtual machine, henceforth referred to H5VM, is a specified execution engine for programs.

***
#### H5VM Memory layout (Code memory and data memory)
Following Harvard architecture principles, H5VM loads code and data separately into two different 'members'. The machine code is loaded into the code member, while the data member contains almost all information that is manipulated during the program execution itself.

The code member is read-only.

The data member has addresses (memory cells indexed from 0 to 0xFFFF) that are either read/write (RW), read-only (RO), or unmapped (UM). These three properties are referred to as the 'permission' of an address.

- The following addresses must be present in the data member with the indicated codename (for later use in this document), index, and permissions:
	- Zero flag (`_ZF`) (0xFFFF) (RW)
	- Carry flag (`_CF`) (0xFFFE) (RW)
	- Input register (`_IN`) (0xFFFD) (RO): Reads to the input register may prompt the user for numbers
	- Output register (`_OU`) (0xFFFD) (RW): Writes to the output register may display the numbers to the user
	- Program counter register low byte (`_PCL`) (0xFFFB) (RO): must contain the low byte of the program counter at ALL TIMES
	- Program counter register high byte (`_PCH`) (0xFFFA) (RO): must contain the high byte of the program counter at ALL TIMES
	- Error address (`_ERR`) (0xFFF9) (UN)
	- General purpose memory (`_GMEM`) (from 0 until 0x3FFF) (RW): General-purpose writable addresses
	- Read-only drive (`_DRIV`) (from 0x4000 until 0xBFFF) (RO): Read-only addresses, intended to be loaded as static data at the beggining of execution

#### Machine code format:
- A program is a sequence of instructions.
- Each instruction is 5 bytes in length, and has 4 parts stored sequentially: the type nibble (4 bits), the opcode (4 bits), the first operand (16 bits), and the second operand (16 bits)
- The type nibble indicates the type of the operands:
	- If the lowest bit is zero, the second operand is an of type ADDRESS, else it is of type `[OPTYPE]`.
	- If the second lowest bit is zero, the first operand is of type ADDRESS, else it is of type `[OPTYPE]`.
	- The third lowest bit is irrelevant.
	- If highest bit is zero, subsitute `[OPTYPE]` above with LITERAL. If it is one, subsitute it with POINTER.
- The opcode indicates the operation to be done, see the explanation of opcodes below.
- The two operands will be interpreted depending on the opcode.
- The program counter is the index of the instruction being currently executed, if one was to assign an index to each instruction. To modify it means to change which instruction executes next. It can only be modified through the opcodes `jmp`, `skpz`, `skmz`, `func`, `ret`, `call`.
- The program counter increases by one after an instruction is executed, except when one of these six opcodes explicitly modifies it (If `_ZF` is not zero upon execution of an `skpz` or an `skmz`, the program counter also just increases by one as usual)

EXAMPLE:

```
BINARY: 0001 0101 000000000000001 000000000011111 0000 0000 0000000000000000 0000000000000000
HEX:    0x01 0x05            0x01            0x1F 0x00 0x00             0x00             0x00
DECIMAL:   1    5               1              31 0       0                0                0
ASSEMBLY:     add               1             =1F      halt
ENGLISH: add the contents of address ONE and the constant 0x1F, put the result in address ONE; stop execution
PROPER ASSEMBLY:
	add 1 =1F
	halt
```

#### Assembly format:
- It is a textual representation of the machine code
- Each opcode is represented by its lowercase name (see opcode guide)
- Each operand's value is represented in raw, uppercase hexadecimal
- Operands are to be directly preceded by:
	- No symbol if they are of type ADDRESS
	- The equal symbol `=` if they are of type LITERAL
	- The asterisk symbol `*` if they are of type POINTER
- The opcode, the first and second operands are separated by a symbol space '` `'

EXAMPLE:

```
add 1 =50
sub *27FA 12
halt
```

#### Opcodes
- There are currently SIXTEEN (16) opcodes, each numbered with the decimal number for use in the opcode nibble.
- By 'address of a pointer' or 'value of a pointer', what is meant is that what will be read is the value of the address pointed two by THAT ADDRESS AND THE NEXT ONE (e.g. *2 denotes the contents of memory address 0x02 and 0x03) TREATED AS A SINGLE 16-BIT NUMBER. THAT NUMBER will be the address that is written to/read from. This is better ilustrated with an example:
```
set 0 =20
set 1 =02
set *0 =30
halt
```
- After this program is ran, address 0x2002 will have the value 0x30
- Notation explanation:
	- Vx: Type of operand can be literal, address or pointer
	- Rx: Type of operand can be address or pointer
	- Lx: Type of operand can only be literal
	- Except where mentioned, the value of a literal is taken as the lowest 8 bits of the operand

0. halt - Stop program execution immediately and permanently

1. jmp `V1` - Jump
	- Change program counter to the value of `V1`
	- SPECIAL EXCEPTION: Takes 16-bit literals. Addresses are treated the same as literals. Pointers are treated as addresses where the highest 8 bits are the value of the address corresponding to the operand, and the lowest 8 bits are the value of (address of the operand)+1

2. skpz `L1` - Skip plus if zero
	- If _ZF has the value 0, add `L1`+1 to program counter.
	- SPECIAL EXCEPTION: Takes 16-bit literals.

3. skmz `L1` - Skip minus if zero
	- If _ZF has the value 0, substract (`L1`+1) from program counter.
	- SPECIAL EXCEPTION: Takes 16-bit literals.

4. set `R1 V2` - Copy the value of `V2` to the address of `R1`.

5. add `R1 V2` - Addition
	- Perform addition between the value of `R1` and `V2`.
	- Put the result into the address of `R1`.
	- If the value 0 was written to the address of `R1`, set _ZF to 0.
	- If the operation results in overflow, set _CF to 1.
	- Otherwise, set _CF to 0.

6. sub `R1 V2` - Substraction
	- Perform substraction between the value of `R1` and `V2`.
	- Put the result into the address of `R1`.
	- If the value 0 was written to the address of `R1`, set _ZF to 0.
	- If the operation results in overflow, set _CF to 1.
	- Otherwise, set _CF to 0.

7. and `R1 V2` - Binary And
	- Perform binary AND with the value of `R1` and `V2`.
	- Put the result into the address of `R1`.
	- If the value 0 was written to the address of `R1`, set _ZF to 0.

8. or `R1 V2` - Inclusive Or
	- Perform inclusive OR with the value of `R1` and `V2`.
	- Put the result into the address of `R1`.
	- If the value 0 was written to the address of `R1`, set _ZF to 0.

9. xor `R1 V2` - Exclusive Or
	- Perform exclusive OR with the value of `R1` and `V2`.
	- Put the result into the address of `R1`.
	- If the value 0 was written to the address of `R1`, set _ZF to 0.

10. shift `R1 V2` - Bitshift
	- If the value of `V2` is 0-7, bitshift the value of `R1` LEFT  by `V2` bits.
	- If the value of `V2` is 8-F, bitshift the value of `R1` RIGHT by (`V2-8`) bits.
	- If the value of `V2` is greater than F, the value of `R1` remains unchanged.
	- Put the result into the address of `R1`.
	- If the value 0 was written to the address of `R1`, set _ZF to 0.

11. cmp `V1 V2` - Comparison
	- Perform substraction between the value of `V1` and `V2`.
	- If the operation results in underflow, set _CF to 1.
	- Otherwise, set _CF to 0.

12. func `L1 L2`
	- Writes down (preferrably in a place not normally addressable) the program counter as (start of subroutine `L1`).
	- Moves the program counter to directly after the closest `ret` instruction.

13. ret `L1 L2`
	- Marks the end of subroutine `L1`.
	- When reached, the current stack frame is deleted.
	- Afterwards, the program counter is set to the return address.

14. call `L1 L2`
	- Move program counter to (start of subroutine `L1`)+1.
	- Creates a new stack frame by increasing the stack pointer at (High nibble of `L2`)`*`4096.
	- The stack frame has size (Low nibble of `L2`)`*`2+2.
	- The first two bytes of the stack frame are the return address.
	- The return address is written down as the program counter corresponding to the instruction after this one.

15. frame `V1 L2`
	- Get current stack frame information
	- (High nibble of `L2*4096`) is the start address of the stack.
	- (Low  nibble of `L2`)`*`2+2   is the size in bytes of each stack frame.
	- The address of the current stack frame is deposited as a 16-bit number to the addresses `V1` and `V1+1`.
