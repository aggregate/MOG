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
ten:
 .word 3
a:
 .word 1109917696
 .space 256
DATASIZE:
 .text
exit:
 sys 0
 .ent fact
fact:
 li -32
 sr $2
 lr $0
 add $2
 sr $0
 li 28
 add $0
 sr $2
 lr $1
 sw $2
 li 32
 add $0
 sr $2
 lr $6
 sw $2
 li 32
 add $0
 lw
 sr $3
 li 2
 sr $2
 lr $3
 slt $2
 sr $3
 jf DOL_L2
 li 1
 sr $3
 li DOL_L3
 sr $2
 j
DOL_L2:
 li 32
 add $0
 lw
 sr $3
 li -1
 sr $2
 lr $3
 add $2
 sr $3
 sr $6
 .ret L_0
 li L_0
 sr $1
 li fact
 sr $2
L_0:
 lr $3
 sr $4
 li 32
 add $0
 sr $2
 lw
 sr $3
 lr $4
 mul $3
 sr $3
DOL_L3:
 li 28
 add $0
 lw
 sr $1
 li 32
 sr $2
 lr $0
 add $2
 sr $0
 lr $1
 j
 .ent main
main:
 li -40
 sr $2
 lr $0
 add $2
 sr $0
 li 36
 add $0
 sr $2
 lr $1
 sw $2
 li 24
 add $0
 sr $2
 li 0
 sw $2
 li DOL_L5
 sr $2
 j
DOL_L6:
 li IPROC
 lw
 sr $4
 li 24
 add $0
 sr $2
 lw
 sr $3
 lr $4
 xor $3
 sr $4
 li 0
 li 2
 sr $3
 jt MYLOCAL_0
MYLOCAL_0:
 lr $4
 rem $3
 sr $3
 sr $6
 .ret L_1
 li L_1
 sr $1
 li fact
 sr $2
L_1:
 li 16
 add $0
 lw
 sr $7
 lr $5
 lr $5
 i2f
 sr $5
 li 0
 li 2
 sr $4
 li 24
 add $0
 lw
 sr $3
 li 1
 sr $2
 lr $3
 add $2
 sr $3
 li 24
 add $0
 sr $2
 lr $3
 sw $2
DOL_L5:
 li 24
 add $0
 lw
 sr $3
 li 2
 sr $2
 lr $3
 slt $2
 sr $3
 jt DOL_L6
 li 36
 add $0
 lw
 sr $1
 li 40
 sr $2
 lr $0
 add $2
 sr $0
 lr $1
 j
