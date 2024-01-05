# Halfive Virtual Machine Specification

#### Copyright Nomagno 2021-2023
#### Copying and distribution of this work, with or without modification, are permitted in any medium without royalty, provided the copyright notice and this notice are preserved. This work is offered as-is, without any warranty.

H5VM assembly is identified by the MIME type `text/x-h5asm`, extension ".h5asm"

H5VM machine code sequences or "executables" are identified by the MIME type `application/x-h5bin`, extension ".h5bin"

H5VM drives are identified by the MIME type `application/x-h5drive`, extension ".h5drive"

### The Halfive virtual machine
The Halfive virtual machine, henceforth referred to H5VM, is a specified execution engine for programs.

***
#### H5VM Memory layout (Code memory and data memory)
Following Harvard architecture principles, H5VM loads code and data separately into two different 'members'. The machine code is loaded into the code member, while the data member contains almost all information that is manipulated during the program execution itself. Both members are big-endian.

The code member is read-only.

The data member has addresses (16-bit unsigned memory cells indexed from 0 to 0xFFFF). Address `0x00` is known as the zero address, and is reserved. While the rest of the memory is completely free to use so long as it is mapped to a physical device, addresses `0x01` through `0xFD` are likely to be used for the jumptable. Reads from address `0xFE` may prompt the user for input, and writes to address `0xFF` may display the written data to the user. "Jumptable address `foo`" is a shorthand for the memory cell with index `jumptable_offset + 1`. `jumptable_offset` has value `0x01` which can not be changed.


There is additionally a register member, which has currently multiple defined registers:
- Flag register (`0x00`) (`F`): This register has multiple flags that are defined later on. Starting from the LEAST significant bit, the meaning of each zero-indexed bit position is:
  * 0 = Zero Flag - Set by arithmetic group operations. If the result is zero, it is set to low, else it is set to high.
  * 1 = Overflow Flag - Set by wraparound group operations. If the operation results in wraparound, set it to high, else set it to low.
  * 2 through 7 - Reserved
  * 8 trough 15 - Implementation-defined
- Program counter register (`0x01`) (`P`): Holds the actual program counter value used to drive the processor. Please modify through the appropiate control flow group and subroutine group operations only.
- Stack pointer register (`0x02`) (`S`): holds the stack pointer, used by multiple operations. It can be modified directly, but using the subroutine group operations `STKI` and `STKD` is highly recommended unless there is a special need.
- Stack size register (`0x03`) (`R`): holds the size of each stack frame, in memory cells.
- General purpose registers `A`, `B`, `C`, `D`, `W`, `X`, `Y`, `Z` (`0x04` through to `0x0B`): Can be written or read from for any purpose, at any time.

#### Machine code format:
- A program is a sequence of instructions.
- Each instruction is 4 octets in length (32 bits), and has four parts stored sequentially (leftmost bit == most significant bit): the opcode (6 bits), the addressing mode (2 bits) (`0b00`=`m`, `0b01`=`i`, `0b10`=`p`, `0b11`=`s`), the register cell (8 bits), and the operand (16 bits)
- The opcode indicates the operation to be done, see the explanation of opcodes below.
- The addressing mode is tied semantically to the opcode and specifies information about to the operation to be done, see the explanation of opcodes below.
- The register cell and operand will be interpreted depending on the opcode, but usually the register cell is representing a register, and the operand is representing an immediate value, address or pointer.
- The program counter is the index of the instruction being currently executed, if one was to assign an index to each instruction. To modify it means to change which instruction executes next. It should only be modified through the opcodes `jmp`, `skpz`, `skmz`, `func`, `ret`, `call`.
- The program counter increases by 1 after an instruction is executed, except when one of these six opcodes explicitly modifies it (If the zero flag is not 0 upon execution of an `skpz` or an `skmz`, the program counter also just increases by one as usual)

#### Assembly format:
- It is a textual representation of the machine code
- Each opcode is represented by its code, in uppercase (see opcode guide)
- Each addressing mode is represented by its code, in lowercase
- Each register cell's value is represented by either two characters of uppercase hexadecimal or `_X`/`X_`, where its register code is `X` and has the case indicated in the memory layout section. If it is irrelevant, it is labelled as `__` in the opcode guide. Note `__` does not stand for a space, but for two literal underscode characters.
- Each operand's value is represented by either two or four characters of uppercase hexadecimal. A two-character register may also be used as representation, though it is suggested this is only done for operations that do, in fact, manipulate two registers at once.
- The opcode and the addressing mode are written together, the register cell and second operands are separated by the space symbol '` `'. The following combinations are accepted in assembly, in which case the non present data will be taken to be all zeroes: opcode, opcode + addressing mode, opcode + addressing mode + register cell, opcode + addressing mode + register cell + two-digit operand, opcode + addressing mode + register cell + four-digit operand
- Instructions may be separated by either a newline or a colon symbol '`:`'
- Comments are defined as all text from the first semicolon symbol `;` in a line, if present, to the end of the line, and they are ignored by the assembler.

#### Opcodes
- Each opcode is numbered with its corresponding decimal number. It corresponds to the representation used in the opcode.
- Opcodes `0` through to `3` conform the control flow group
- Opcodes `4` through to to `8` conform the subroutine group
- Opcodes `9` through to `12` conform the memory management group

	- Address mode `m`:
	- Immediate mode `i`:
	- Pointer mode `p`:
	- Special mode mode `s`:

0. HALT
	- Stop program execution.

1. JUMP `__` `OP` - Jump
	- Address mode `m`: Set program counter to the value of address `OP`.
	- Immediate mode `i`: Set program counter to the immediate value `OP`.
	- Pointer mode `p`: Set program counter to by dereference of `OP`.
	- Special mode mode `s`: Not supported.

2. SKPZ `__` `OP` - Skip plus if zero
	- Default mode `m`: Same as `i`.
	- Immediate mode `i`: If the zero flag is set to low, add to the program counter the immediate value `OP`, plus one. Else, do nothing.
	- Pointer mode `p`: Not supported.
	- Special mode mode `s`: Not supported.

3. SKMZ `__` `OP` - Skip minus if zero
	- Default mode `m`: Same as `i`.
	- Immediate mode `i`: If the zero flag is set to low, substract from the program counter [the immediate value `OP`, minus one] (pay attention to bracket positioning). Else, do nothing.
	- Pointer mode `p`: Not supported.
	- Special mode mode `s`: Not supported.

4. STKI
	- Increase stack pointer by the value of the stack size register. Overflow may happen.
5. STKD
	- Decrease stack pointer by the value of the stack size register. Underflow may happen.

6. FUNC `_X` `OP` - Register subroutine
	- Default mode `m`: Same as `i`.
	- Immediate mode `i`: Set the jumptable address `OP` to the current program counter plus one.
	- Pointer mode `p`: Not supported.
	- Special mode mode `s`: Set the jumptable address [value of address `OP`] to the value of register `X`.

7. BACK - End of subroutine
	- Set the program counter to the return program counter.

8. CALL `__` `OP` - Call subroutine
	- Default mode `m`: Same as `i`
	- Immediate mode `i`: Set the address pointed to by the stack pointer to the current program counter plus one. This is known as the return program counter. Then set the program counter to the value of jumptable address `OP`.
	- Pointer mode `p`: Not supported.
	- Special mode `s`: Set the address pointed to by the stack pointer to the current program counter plus one. This is known as the return program counter. Then set the program counter to the value of (normal, not jumptable!) address `OP`.

9. MLOD `_X` `OP` - Memory load
	- Address mode `m`: Copy value of address `OP` to register `X`
	- Immediate mode `i`: Copy immediate value `OP` to register `X`
	- Pointer mode `p`: Copy dereference of `OP` to register `X`
	- Special mode mode `s`: Copy register `OP` to register `X`

10. VLOD `_X` `OP` - Variable load
	- Address mode `m`: Copy register `X` to the local variable address obtained by adding [the value of address `OP`] to the current stack pointer value.
	- Immediate mode `i`: Copy register `X` to the local variable address obtained by adding [immediate value `OP`] to the current stack pointer value.
	- Pointer mode `p`: Unsupported mode
	- Special mode mode `s`: Unsupported mode

11. MSTR `_X` `OP` - Memory store
	- Address mode `m`: Copy register `X` to address `OP`
	- Immediate mode `i`: Unsupported mode
	- Pointer mode `p`: Copy register `X` to the address pointed to by `OP`
	- Special mode mode `s`: Unsupported mode

12. VSTR `_X` `OP` - Variable store
	- Address mode `m`: Copy the value of [the local variable address obtained by adding [the value of address `OP`] to the current stack pointer value] to register `X`.
	- Immediate mode `i`: Copy the value of [the local variable address obtained by adding [immediate value `OP`] to the current stack pointer value] to register `X`.
	- Pointer mode `p`: Unsupported mode
	- Special mode mode `s`: Unsupported mode

13. FSET `__` `OP` - Set flag
	- Set bit with index `OP` (starting with the least significant bit) of the flag register to high.
14. FDEL `__` `OP` - Delete flag
	- Set bit with index `OP` (starting with the least significant bit) of the flag register to low.
15. FFLP `__` `OP` - Flip flag
	- Set bit with index `OP` (starting with the least significant bit) of the flag register to low if it is currently high, and to high if it is currently low.

16. BADD - Unsigned addition
	- Behaves exactly like `MLOD`, but:
	- Sets the zero flag to low if the operation resulted in the value zero, and to high otherwise.
	- Sets the overflow flag to high if the operation resulted in overflow, and to low otherwise.
	- Instead of simply copying a value to the register, it performs binary addition with the value and the register's current value, and copies the result to the register.

17. BSUB - Unsigned substraction
	- Behaves exactly like `MLOD`, but:
	- Sets the zero flag to low if the operation resulted in the value zero, and to high otherwise.
	- Sets the overflow flag to high if the operation resulted in underflow, and to low otherwise.
	- Instead of simply copying a value to the register, it substracts the value from the register's current value, and copies the result to the register.

18. BAND - Bitwise and
	- Behaves exactly like `MLOD`, but:
	- Sets the zero flag to low if the operation resulted in the value zero, and to high otherwise.
	- Instead of simply copying a value to the register, it performs bitwise "and" with the value and the register's current value, and copies the result to the register.

19. INOR - Inclusive or
	- Behaves exactly like `MLOD`, but:
	- Sets the zero flag to low if the operation resulted in the value zero, and to high otherwise.
	- Instead of simply copying a value to the register, it performs bitwise "or" with the value and the register's current value, and copies the result to the register.

20. EXOR - Exclusive or
	- Behaves exactly like `MLOD`, but:
	- Sets the zero flag to low if the operation resulted in the value zero, and to high otherwise.
	- Instead of simply copying a value to the register, it performs bitwise "xor" with the value and the register's current value, and copies the result to the register.

21. SHFT - Shift bits
	- Behaves exactly like `MLOD`, but:
	- Sets the zero flag to low if the operation resulted in the value zero, and to high otherwise.
	- Instead of simply copying a value to the register, it performs bitwise "xor" with the value and the register's current value, and copies the result to the register.

22. COMP - Comparison
	- Behaves exactly like `BSUB`, but merely sets the flags and doesn't actually modify any other place in memory or the registers, discarding the result.
