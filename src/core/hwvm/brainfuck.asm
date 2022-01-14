; Brainfuck -> HWVM Assembly preprocessor set

; Run 'asmpp.sh 'file' > 'file_p' to
; preprocess this file into assembler-ready format
; (Preprocess #d directives, remove comments, remove empty lines, replace INSERT_NEWLINE)

#d START,set =1 103

#d >,add 101 103 101__
#d <,sub 101 103 101__

#d +,add &100 103 &100__
#d -,sub &100 103 &100__

#d !,set &100 FFFC__
#d ?,set FFFD &100__

#d (,set FFFB 102__

#d ),BACK1__BACK2__

#d BACK1,set &100 FFFF__
#d BACK2,jcnz 102__

#d __,INSERT_NEWLINE


;EXAMPLE PROGRAM

START

++ ; Cell 0 is =2
! ; Print to verify

>+++++ ; Cell 1 is =5
! ; Print to verify

( ; Loop
<+ ; Add one to cell 0
>- ; Substract one to cell 1
) ; If cell 1 is NOT zero, loop

<! ; Else print cell 0 (should be 7)
