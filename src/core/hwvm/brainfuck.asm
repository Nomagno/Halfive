; Brainfuck -> HWVM Assembly preprocessor set

; Run 'asmpp.sh 'file' > 'file_p' to
; preprocess this file into assembler-ready format
; (Preprocess #d directives, remove comments, remove empty lines)

; Ignore __

#d >,INCR
#d <,DECR
#d +,ADD
#d -,SUB
#d !,PRINT
#d ?,GET
#d (,WHILE
#d ),BACK1 __ BACK2

#d START,set [1] 103

#d INCR,add 101 103 101
#d DECR,sub 101 103 101

#d ADD,add {100} 103 {100}
#d SUB,sub {100} 103 {100}

#d PRINT,set {100} FFFC
#d GET,set FFFD {100}

#d WHILE,set FFFB 102
#d BACK1,set {100} FFFF
#d BACK2,jcnz 102

#d __,INSERT_NEWLINE

;EXAMPLE PROGRAM

START

+
+ ; Cell 0 is [2]
! ; Print to verify

>
+
+
+
+
+ ; Cell 1 is [5]
! ; Print to verify

( ; Loop

<
+ ; Add one to cell 0

>
- ; Substract one to cell 1

) ; If cell 1 is NOT zero, loop

<
! ; Else print cell 0 (should be 7)
