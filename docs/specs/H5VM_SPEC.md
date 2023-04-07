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

The data member has addresses (16-bit unsigned memory cells indexed from 0 to 0xFFFF) that are either read/write (RW), read-only (RO), or unmapped (UM). These three properties are referred to as the 'permission' of an address.

- The following addresses must be present in the data member with the indicated codename (for later use in this document), index, and permissions:
	- Zero flag (`_ZF`) (0xFFFF) (RW)
	- Carry flag (`_CF`) (0xFFFE) (RW)
	- Input register (`_IN`) (0xFFFD) (RO): Reads to the input register may prompt the user for numbers
	- Output register (`_OU`) (0xFFFC) (RW): Writes to the output register may display the numbers to the user. Reads always yield the value zero
	- Program counter register (`_PCH`) (0xFFFA) (RO): The program counter value at ALL TIMES
	- Error address (`_ERR`) (0xFFF0) (UM)
	- General purpose memory (`_GMEM`) (from 0 until 0x3FFF) (RW): General-purpose writable addresses
	- Read-only drive (`_DRIV`) (from 0x4000 until 0xBFFF) (RO): Read-only addresses, intended to be loaded as static data at the beggining of execution

#### Machine code format:
- A program is a sequence of instructions.
- Each instruction is 5 octets in length (40 bits), and has 4 parts stored sequentially: the type nibble (4 bits), the opcode (4 bits), the first operand (16 bits), and the second operand (16 bits)
- The type nibble indicates the type of the operands:
	- If the lowest bit is zero, the second operand is an of type ADDRESS, else it is of type `[OPTYPE]`.
	- If the second lowest bit is zero, the first operand is of type ADDRESS, else it is of type `[OPTYPE]`.
	- The third lowest bit is irrelevant.
	- If highest bit is zero, subsitute `[OPTYPE]` above with CONSTANT. If it is one, subsitute it with DERREFERENCE.
	- DERREFERENCES hold a 16-bit memory address that is accessed, and the value of that memory address is the memory cell the instruction operares on.
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
	- The equal symbol `=` if they are of type CONSTANT
	- The asterisk symbol `*` if they are of type DERREFERENCE
- The opcode, the first and second operands are separated by a symbol space '` `'

EXAMPLE:

```
add 1 =50
sub *27FA 12
halt
```

#### Opcodes
- There are currently SIXTEEN (16) opcodes, each numbered with the decimal number for use in the opcode nibble.
- By address of/value of a DERREFERENCE, what is meant is that what will be read is the value of the address pointed to by THAT ADDRESS (e.g. *2 denotes the contents of memory address 0x0002 TREATED AS AN ADDRESS.. THAT will be the address that is written to/read from). This is better ilustrated with an example:
```
set 0 =0020
set *0 =30
halt
```
- After this program is ran, address 0x0020 will have the value 0x30
- Notation explanation:
	- Vx: Type of operand can be constant, address or derreference
	- Rx: Type of operand can be address or derreference
	- Cx: Type of operand can only be constant
	- Except where mentioned, the value of a constant is taken as the lowest 8 bits of the operand

0. halt - Stop program execution immediately and permanently

1. jmp `V1` - Jump
	- Change program counter to the value of `V1`
	- Addresses are treated the same as constants. Derreferences are treated as pointers to the address containing the program counter to set.

2. skpz `C1` - Skip plus if zero
	- If _ZF has the value 0, add `C1`+1 to program counter.

3. skmz `C1` - Skip minus if zero
	- If _ZF has the value 0, substract (`C1`+1) from program counter.

4. set `R1 V2` - Copy the value of `V2` to the address of `R1`.

5. add `R1 V2` - Addition
	- Perform addition between the value of `R1` and `V2`.
	- Put the result into the address of `R1`.
	- If the value 0 was written to the address of `R1`, set _ZF to 0.
	- Otherwise, set _ZF to 1.
	- If the operation results in overflow, set _CF to 1.
	- Otherwise, set _CF to 0.

6. sub `R1 V2` - Substraction
	- Perform substraction between the value of `R1` and `V2`.
	- Put the result into the address of `R1`.
	- If the value 0 was written to the address of `R1`, set _ZF to 0.
	- Otherwise, set _ZF to 1.
	- If the operation results in overflow, set _CF to 1.
	- Otherwise, set _CF to 0.

7. and `R1 V2` - Binary And
	- Perform binary AND with the value of `R1` and `V2`.
	- Put the result into the address of `R1`.
	- If the value 0 was written to the address of `R1`, set _ZF to 0.
	- Otherwise, set _ZF to 1.

8. or `R1 V2` - Inclusive Or
	- Perform inclusive OR with the value of `R1` and `V2`.
	- Put the result into the address of `R1`.
	- If the value 0 was written to the address of `R1`, set _ZF to 0.
	- Otherwise, set _ZF to 1.

9. xor `R1 V2` - Exclusive Or
	- Perform exclusive OR with the value of `R1` and `V2`.
	- Put the result into the address of `R1`.
	- If the value 0 was written to the address of `R1`, set _ZF to 0.
	- Otherwise, set _ZF to 1.

10. shift `R1 V2` - Bitshift
	- If the value of `V2` is 0x00-0x0F, bitshift the value of `R1` LEFT  by `V2` bits.
	- If the value of `V2` is 0x10-0x1F, bitshift the value of `R1` RIGHT by (`V2-0x10`) bits.
	- If the value of `V2` is greater than 0x1F, the value of `R1` remains unchanged.
	- Put the result into the address of `R1`.
	- If the value 0 was written to the address of `R1`, set _ZF to 0.
	- Otherwise, set _ZF to 1.

11. cmp `V1 V2` - Comparison
	- Perform substraction between the value of `V1` and `V2`.
	- If the value 0 was written to the address of `R1`, set _ZF to 0.
	- Otherwise, set _ZF to 1.
	- If the operation results in underflow, set _CF to 1.
	- Otherwise, set _CF to 0.

12. func `C1`
	- Writes down (preferrably in a place not normally addressable) the program counter as (start of subroutine `C1`).
	- Moves the program counter to directly after the closest `ret` instruction.

13. ret `C1 C2`
	- Marks the end of subroutine `C1`.
	- When reached, the current stack frame is deleted.
	- Afterwards, the program counter is set to the return address.

14. call `C1 C2`
	- Move program counter to (start of subroutine `C1`)+1.
	- Creates a new stack frame by increasing the stack pointer at (High octet of `C2`)`*`256.
	- The stack frame has size (Low octet of `C2` +  1) in memory cells.
	- The first memory cell of the stack frame is the return address.
	- The return address is written down as the program counter corresponding to the instruction after this one.

15. frame `R1 C2`
	- Get current stack frame information
	- (High octet of `C2`)`*`256 is the start address of the stack.
	- (Low  octet of `C2` + 1) is the size in memory cells of each stack frame.
	- The address of the current stack frame is deposited as a 16-bit number to the address `R1`.
