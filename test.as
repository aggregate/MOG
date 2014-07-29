 .data
NPROC:
 .word 0
IPROC:
 .word 0
NSYSARGS:
 .word 32
ISYSARGS:
 .word 0
SYSARGS:
 .space 128
a:
 .space 4
b:
 .space 4
 .space 256
DATASIZE:
 .text
 .ent main
main:
 li -40
 sr $3
 lr $0
 add $3
 sr $0
 li 36
 add $0
 sr $3
 lr $1
 sw $3
 li 97
 sr $2
 li 24
 add $0
 sr $3
 lr $2
 sb $3
 li 98
 sr $2
 li 25
 add $0
 sr $3
 lr $2
 sb $3
 li 99
 sr $2
 li 26
 add $0
 sr $3
 lr $2
 sb $3
 li 27
 add $0
 sr $3
 li 0
 sb $3
 li 24
 sr $3
 lr $0
 add $3
 sr $2
 sr $5
 li 3
 sr $6
 .ret L_0
 li L_0
 sr $1
 li open
 sr $3
 j
L_0:
 lr $7
 sr $5
 .ret L_1
 li L_1
 sr $1
 li exit
 sr $3
 j
L_1:
 .ent exit
exit:
 lr $5
 sw $0
 li NSYSARGS
 sr $2
 li 3
 sw $2
 li 16
 sr $2
 li 0
 sw $2
 li 20
 sr $2
 lr $0
 lw
 sr $4
 sw $2
 sys 0
 lr $1
 j
 .ent time
time:
 lr $5
 sw $0
 li NSYSARGS
 sr $2
 li 3
 sw $2
 li 16
 sr $2
 li 1
 sw $2
 li 32
 sr $2
 lr $0
 lw
 sr $4
 sw $2
 sys 0
 li 16
 lw
 sr $2
 lr $1
 j
 .ent dup
dup:
 lr $5
 sw $0
 li NSYSARGS
 sr $2
 li 3
 sw $2
 li 16
 sr $2
 li 2
 sw $2
 li 20
 sr $2
 lr $0
 lw
 sr $4
 sw $2
 sys 0
 li 16
 lw
 sr $2
 lr $1
 j
 .ent putchar
putchar:
 lr $5
 sb $0
 li NSYSARGS
 sr $2
 li 2
 sw $2
 li 16
 sr $2
 li 3
 sw $2
 li 20
 sr $2
 lr $0
 lb
 sr $4
 sw $2
 sys 0
 lr $1
 j
 .ent getchar
getchar:
 li NSYSARGS
 sr $2
 li 2
 sw $2
 li 16
 sr $2
 li 4
 sr $4
 sw $2
 sys 0
 li 16
 lw
 sr $2
 lr $1
 j
 .ent open
open:
 lr $5
 sw $0
 li 4
 add $0
 sr $3
 lr $6
 sw $3
 li 16
 sr $2
 li 5
 sw $2
 li NSYSARGS
 sr $2
 li 1
 sr $4
 sw $2
 li DOL_L11
 sr $3
 j
DOL_L12:
 li NSYSARGS
 lw
 sr $2
 li 4
 sr $3
 lr $2
 add $3
 sr $2
 li 2
 sr $3
 lr $2
 sll $3
 sr $2
 li NSYSARGS
 lw
 sr $4
 li -1
 sr $3
 lr $4
 add $3
 sr $4
 lr $0
 lw
 sr $5
 add $4
 lb
 sw $2
 li NSYSARGS
 lw
 sr $2
 li 1
 sr $3
 lr $2
 add $3
 sr $4
 li NSYSARGS
 sr $2
 lr $4
 sw $2
DOL_L11:
 li NSYSARGS
 lw
 sr $2
 li -1
 sr $3
 lr $2
 add $3
 sr $2
 lr $0
 lw
 sr $4
 add $2
 lb
 sr $2
 jt DOL_L12
 li NSYSARGS
 lw
 sr $2
 li 4
 sr $3
 lr $2
 add $3
 sr $2
 li 2
 sr $3
 lr $2
 sll $3
 sr $2
 li 0
 sw $2
 li NSYSARGS
 lw
 sr $2
 li 1
 sr $3
 lr $2
 add $3
 sr $4
 li NSYSARGS
 sr $2
 lr $4
 sw $2
 li NSYSARGS
 lw
 sr $2
 li 4
 sr $3
 lr $2
 add $3
 sr $2
 li 2
 sr $3
 lr $2
 sll $3
 sr $2
 li 4
 add $0
 lw
 sw $2
 li NSYSARGS
 lw
 sr $2
 li 1
 sr $3
 lr $2
 add $3
 sr $4
 li NSYSARGS
 sr $2
 lr $4
 sw $2
 sys 0
 li 16
 lw
 sr $2
 lr $1
 j
 .ent read
read:
 lr $5
 sw $0
 li 4
 add $0
 sr $3
 lr $6
 sw $3
 li 8
 add $0
 sr $3
 lr $8
 sw $3
 li 16
 sr $2
 li 6
 sw $2
 li NSYSARGS
 sr $2
 li 1
 sw $2
 li NSYSARGS
 lw
 sr $2
 li 4
 sr $3
 lr $2
 add $3
 sr $2
 li 2
 sr $3
 lr $2
 sll $3
 sr $2
 lr $0
 lw
 sw $2
 li NSYSARGS
 lw
 sr $2
 li 1
 sr $3
 lr $2
 add $3
 sr $4
 li NSYSARGS
 sr $2
 lr $4
 sw $2
 sys 0
 li 16
 lw
 sr $2
 lr $1
 j
