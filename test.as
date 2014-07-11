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
 li -32
 sr $3
 lr $0
 add $3
 sr $0
 li 28
 add $0
 sr $3
 lr $1
 sw $3
 .ret L_0
 li L_0
 sr $1
 li getchar
 sr $3
 j
L_0:
 li 16
 add $0
 lw
 sr $6
 lr $2
 sr $4
 li a
 sr $2
 lr $4
 sw $2
 li a
 lw
 sr $2
 li 24
 sr $3
 lr $2
 sll $3
 sr $2
 li 24
 sr $3
 lr $2
 sra $3
 sr $2
 sr $5
 .ret L_1
 li L_1
 sr $1
 li putchar
 sr $3
 j
L_1:
 li 16
 add $0
 lw
 sr $6
 lr $7
 sr $5
 .ret L_2
 li L_2
 sr $1
 li exit
 sr $3
 j
L_2:
 .ent exit
exit:
 lr $5
 sw $0
 li NSYSARGS
 sr $2
 li 16
 sw $2
 li 16
 sr $2
 li 0
 sw $2
 li 20
 sr $2
 lr $0
 lw
 sw $2
 li 24
 sr $2
 li 555
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
 li 16
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
 li 8
 sw $2
 li 16
 sr $2
 li 2
 sw $2
 li 20
 sr $2
 lr $0
 lw
 sw $2
 li 24
 sr $2
 li 333
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
