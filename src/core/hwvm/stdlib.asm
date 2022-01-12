; Position-independent HWVM assembly stdlib

; Run 'asmpp.sh 'file' > 'file_p' to
; preprocess this file into assembler-ready format
; (Preprocess #d directives, remove comments, remove empty lines)


;LICENSE START START START START
;Copyright Nomagno 2021

;Redistributions, modified or unmodified, in whole or in part, must retain
;applicable notices of copyright or other legal privilege, these conditions, and
;the following license terms and disclaimer.  Subject to these conditions, each
;holder of copyright or other legal privileges, author or assembler, and
;contributor of this work, henceforth "licensor", hereby grants to any person
;who obtains a copy of this work in any form:

;1. Permission to reproduce, modify, distribute, publish, sell, sublicense, use,
;and/or otherwise deal in the licensed material without restriction.

;2. A perpetual, worldwide, non-exclusive, royalty-free, gratis, irrevocable
;patent license to make, have made, provide, transfer, import, use, and/or
;otherwise deal in the licensed material without restriction, for any and all
;patents held by such licensor and necessarily infringed by the form of the work
;upon distribution of that licensor's contribution to the work under the terms
;of this license.

;NO WARRANTY OF ANY KIND IS IMPLIED BY, OR SHOULD BE INFERRED FROM, THIS LICENSE
;OR THE ACT OF DISTRIBUTION UNDER THE TERMS OF THIS LICENSE, INCLUDING BUT NOT
;LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,
;AND NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS, ASSEMBLERS, OR HOLDERS OF
;COPYRIGHT OR OTHER LEGAL PRIVILEGE BE LIABLE FOR ANY CLAIM, DAMAGES, OR OTHER
;LIABILITY, WHETHER IN ACTION OF CONTRACT, TORT, OR OTHERWISE ARISING FROM, OUT
;OF, OR IN CONNECTION WITH THE WORK OR THE USE OF OR OTHER DEALINGS IN THE
;WORK.
;LICENSE END END END END

; All numbers here are expressed in hexadecimal base,
; unless otherwise stated

; Addresses 0-F are reserved for subroutine arguments
; Addresses 10-FF are reserved for subroutine internal usage
; Subroutine IDs 0-FF are reserved for stdlib
#d MEMSET,=0
#d MEMCPY,=1


subs MEMSET
; ID: 0
; Instruction count (DECIMAL): 10
; Memset subroutine interface addresses:
; Value to set - 0
; Start range - &1 (1 and 2 treated as 16-bit)
; End range - &3 (3, 4 as 16-bit)

; Implementation-reserved addresses:
; Internal zero flag storage - 10
; Internal Instruction Pointer storage - 11
add FFFB =1 11 ; This stores the relative inst ptr
; that marks the start of the loop
set 0 &1 ; Start of loop, set address to value
add =1 2 2 ; Add one to LSB of address
add FFFE 1 1 ; Add carry to MSB of address
cmp 4 2 ; Compare current LSB with end LSB
set FFFF 10 ; Store result to address 10
cmp 3 1 ; Compare current MSB with end MSB
or FFFF 10 FFFF ; UNLESS both cmps were true, zf=1
jcnz 11 ; IF zf is 1, loop
set 0 &1
sube MEMSET ; ELSE return


subs MEMCPY
; ID: 1
; Instruction count (DECIMAL): 21
; Memcpy subroutine interface addresses:
; Origin range start - &1 (1 and 2 treated as 16-bit)
; Destination range start - &3 (3, 4 as 16-bit)
; Destination range end - &5 (5, 6 as 16-bit)

; Implementation-reserved addresses:
; Store objective range - &10 (10, 11 as 16-bit)
; Store carry/zero flag - 12
; Store Intruction Pointer - 13

; RESTRICTION: THE ORIGIN RANGE START PLUS
; THE SIZE OF THE DESTINATION RANGE MUST NOT
; OVERLAP WITH ANY POINT OF THE DESTINATION RANGE.
; RESTRICTION: THE DESTINATION RANGE START MUST BE EARLIER THAN THE END
; ON MEMORY.
; RESTRICTION: THE DESTINATION RANGE MUST BE COMPLETELY WRITABLE
; NOTE: THE ORIGIN RANGE ENDS AT THE DISTANCE FROM THE DESTINATION RANGE
; START TO ITS END, THAT IS ORIGEND = ((DESTEND - DESTSTART) + ORIGSTART)

add FFFB =9 13 ; Set Instruction Pointer that will
; mark start of loop
; Compute size of destination range
sub 6 4 11 ;Substract LSBs
set FFFE 12 ; Save carry flag
sub 5 3 10 ; Substract MSBs
sub 10 12 10 ; Substract carry from result
; Add destination size to start address
add 2 11 11 ; Add LSBs
set FFFE 12 ; Save carry flag
add 1 10 10 ; Add MSBs
add 12 10 10 ; Add carry to result
set &1 &3 ; Set current origin address value to
;current destination address value
add =1 2 2 ; Shift origin address LSB
add FFFE 1 1 ; Add carry to MSB
add =1 4 4 ; Shift destination address LSB
add FFFE 3 3 ; Add carry to MSB
cmp 11 2 ; Compare objective LSB to current
set FFFF 12 ; Save result to address 12
cmp 10 1 ; Compare objective MSB to current
or FFFF 12 FFFF ; UNLESS they are equal
jcnz 13 ; Loop
set &1 &3
sube MEMCPY ; ELSE return
