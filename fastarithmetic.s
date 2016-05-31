	.file	"fastarithmetic.c"
.text
	.balign 2
	.global	sqac48_16_fast
sqac48_16_fast:
; start of function
; framesize_regs:     2
; framesize_locals:   0
; framesize_outgoing: 0
; framesize:          2
; elim ap -> fp       4
; elim fp -> sp       0
; saved regs: R4
	; start of prologue
	PUSHM.W	#1, R4
	; end of prologue
 ; 12 "toolkit/fastarithmetic.c" 1
	__MPYS=0x132 
__OP2=0x138 
__RESLO=0x13a 
__RESHI=0x13c 
 mov	R13, &__MPYS 
 mov	R13, &__OP2 
 mov.w	#__RESLO, R4 
 add.w   @R4+,0(R12) 
 addc.w  @R4+,2(R12) 
 addc.w  @R4,4(R12) 

 ; 0 "" 2
	; start of epilogue
	POPM.W	#1, r4
	RET
	.size	sqac48_16_fast, .-sqac48_16_fast
	.balign 2
	.global	sqac64_24_fast
sqac64_24_fast:
; start of function
; framesize_regs:     0
; framesize_locals:   6
; framesize_outgoing: 0
; framesize:          6
; elim ap -> fp       2
; elim fp -> sp       6
; saved regs:(none)
	; start of prologue
	SUB.W	#6, R1
	; end of prologue
	MOV.W	R12, 4(R1)
	MOV.W	R13, @R1
	MOV.W	R14, 2(R1)
	CMP.W	#0, 2(R1) { JGE	.L3
	MOV.W	#0, R12
	MOV.W	#0, R13
	MOV.W	R12, R14
	MOV.W	R13, R15
	SUB	@R1, R14 { SUBC	2(R1), R15
	MOV.W	R14, @R1
	MOV.W	R15, 2(R1)
.L3:
	MOV.W	4(R1), R14
	MOV.W	@R1, R12
	MOV.W	2(R1), R13
 ; 42 "toolkit/fastarithmetic.c" 1
	__MPY=0x130 
__OP2=0x138 
__RESLO=0x13a 
__RESHI=0x13c 
 mov	R12, &__MPY 
 mov	R12, &__OP2 
 add	&__RESLO, @R14 
 addc	&__RESHI, 2(R14) 
 adc	3(R14) 
 adc	4(R14) 
 mov	R13, &__OP2 
 add	&__RESLO, 2(R14) 
 addc	&__RESHI, 3(R14) 
 adc	4(R14) 
 add	&__RESLO, 2(R14) 
 addc	&__RESHI, 3(R14) 
 adc	4(R14) 
 mov	R13, &__MPY 
 mov	R13, &__OP2 
 add	&__RESLO, 3(R14) 
 addc	&__RESHI, 4(R14) 

 ; 0 "" 2
	; start of epilogue
	ADD.W	#6, R1
	RET
	.size	sqac64_24_fast, .-sqac64_24_fast
	.balign 2
	.global	mac64_16_24_fast
mac64_16_24_fast:
; start of function
; framesize_regs:     2
; framesize_locals:   8
; framesize_outgoing: 0
; framesize:          10
; elim ap -> fp       4
; elim fp -> sp       8
; saved regs: R4
	; start of prologue
	PUSHM.W	#1, R4
	SUB.W	#8, R1
	; end of prologue
	MOV.W	R12, 6(R1)
	MOV.W	R13, 4(R1)
	MOV.W	R14, @R1
	MOV.W	R15, 2(R1)
	MOV.W	6(R1), R14
	MOV.W	4(R1), R15
	MOV.W	@R1, R12
	MOV.W	2(R1), R13
 ; 81 "toolkit/fastarithmetic.c" 1
	__MPY=0x130 
__OP2=0x138 
__RESLO=0x13a 
__RESHI=0x13c 
 clr	R4 
 tst	R13 
 jge	mac64_16_24_1 
 xor	#1,R4 
 inv	R13 
 inv	R12 
 add	#1,R12 
 adc	R13 
mac64_16_24_1: 
 tst	R15 
 jge	mac64_16_24_2 
 xor	#1,R4 
 inv	R15 
 add	#1,R15 
mac64_16_24_2: 
 mov   R15,&__MPY 
 tst	R4 
 jeq	mac64_16_24_3 
 mov.w	R13,&__OP2 
 mov.w	#__RESLO,R4 
 sub.w	@R4+,2(R14) 
 subc.w	@R4,3(R14) 
 subc.w	#0,4(R14) 
 mov.w   R12,&__OP2 
 sub.w   #2,R4 
 sub.w	@R4+,@R14 
 subc.w  @R4,2(R14) 
 subc.w	#0,3(R14) 
 subc.w	#0,4(R14) 
 jmp	done 
mac64_16_24_3: 
 mov	R12, &__OP2 
 add	&__RESLO, @R14 
 addc	&__RESHI, 2(R14) 
 adc	3(R14) 
 adc	4(R14) 
 mov	R13, &__OP2 
 add	&__RESLO, 2(R14) 
 addc	&__RESHI, 3(R14) 
 adc	4(R14) 
done: 

 ; 0 "" 2
	; start of epilogue
	ADD.W	#8, R1
	POPM.W	#1, r4
	RET
	.size	mac64_16_24_fast, .-mac64_16_24_fast
	.balign 2
	.global	mul48_32_16_fast
mul48_32_16_fast:
; start of function
; framesize_regs:     2
; framesize_locals:   6
; framesize_outgoing: 0
; framesize:          8
; elim ap -> fp       4
; elim fp -> sp       6
; saved regs: R4
	; start of prologue
	PUSHM.W	#1, R4
	SUB.W	#6, R1
	; end of prologue
	MOV.W	R12, 2(R1)
	MOV.W	R13, 4(R1)
	MOV.W	R14, @R1
	MOV.W	2(R1), R12
	MOV.W	4(R1), R13
	MOV.W	@R1, R14
 ; 176 "toolkit/fastarithmetic.c" 1
	__MPY=0x130 
__MPYS=0x132 
__MAC=0x134 
__MACS=0x136 
__OP2=0x138 
__RESLO=0x13a 
__RESHI=0x13c 
__SUMEXT=0x13e 
 push.w  SR 
 dint 
 tst.w	R13 
 jge	mul48_32_16_1 
 inv.w   R12 
 inv.w   R13 
 add.w   #1,R12 
 addc.w  #0,R13 
 mov.w   R14,&__MPY 
 mov.w   R12,&__OP2 
 nop 
 mov.w   &__RESLO,R14 
 mov.w   &__RESHI,R12
 mov.w   R13,&__OP2
 mov.w   #0,R13
 add.w   &__RESLO,R12
 addc.w  &__RESHI,R13
 pop.w   SR 
 inv.w   R14 
 inv.w   R12 
 inv.w   R13 
 add.w   #1,R14 
 addc.w  #0,R12 
 addc.w  #0,R13 
 jmp 	mul48_32_16_done 
mul48_32_16_1: 
 mov.w   R14,&__MPY
 mov.w   R12,&__OP2
 nop 
 nop 
 mov.w   &__RESHI,R12 
 mov.w   R13,&__OP2 
 mov.w   #0,R13 
 add.w   &__RESLO,R12
 addc.w  &__RESHI,R13
 pop.w   SR 
mul48_32_16_done: 

 ; 0 "" 2
	MOV.W	R12, 2(R1)
	MOV.W	R13, 4(R1)
	MOV.W	R14, @R1
	MOV.W	2(R1), R12
	MOV.W	4(R1), R13
	; start of epilogue
	ADD.W	#6, R1
	POPM.W	#1, r4
	RET
	.size	mul48_32_16_fast, .-mul48_32_16_fast
	.balign 2
	.global	mul32
mul32:
; start of function
; framesize_regs:     8
; framesize_locals:   16
; framesize_outgoing: 0
; framesize:          24
; elim ap -> fp       10
; elim fp -> sp       16
; saved regs: R4 R8 R9 R10
	; start of prologue
	PUSHM.W	#3, R10
	PUSHM.W	#1, R4
	SUB.W	#16, R1
	; end of prologue
	MOV.W	R12, 4(R1)
	MOV.W	R13, 6(R1)
	MOV.W	R14, @R1
	MOV.W	R15, 2(R1)
 ; 240 "toolkit/fastarithmetic.c" 1
	__MPY=0x130 
__MPYS=0x132 
__MAC=0x134 
__MACS=0x136 
__OP2=0x138 
__RESLO=0x13a 
__RESHI=0x13c 
__SUMEXT=0x13e 
 mov	R12, &__MPY 
 mov	R14, &__OP2 
 mov	R13, R4 
 mov	&__RESLO, R12 
 mov	&__RESHI, R13 
 mov	R15, &__OP2 
 mov	R4, &__MAC 
 mov	R14, &__OP2 
 mov	#0, R14 
 mov	R15, &__MPY 
 mov	#0, R15 
 add	&__RESLO, R13 
 addc	&__RESHI, R14 
 addc	&__SUMEXT, R15 
 mov	R4, &__OP2 
 add	&__RESLO, R14 
 addc	&__RESHI, R15 

 ; 0 "" 2
	MOV.W	R12, 8(R1)
	MOV.W	R13, 10(R1)
	MOV.W	R14, 12(R1)
	MOV.W	R15, 14(R1)
	MOV.W	8(R1), R12
	MOV.W	10(R1), R13
	MOV.W	12(R1), R14
	MOV.W	14(R1), R15
	MOV.W	R12, R8
	MOV.W	R13, R9
	MOV.W	R14, R10
	MOV.W	R15, R11
	MOV.W	R8, R12
	MOV.W	R9, R13
	MOV.W	R10, R14
	MOV.W	R11, R15
	; start of epilogue
	ADD.W	#16, R1
	POPM.W	#1, r4
	POPM.W	#3, r10
	RET
	.size	mul32, .-mul32
	.balign 2
	.global	dc_filter
dc_filter:
; start of function
; framesize_regs:     0
; framesize_locals:   8
; framesize_outgoing: 0
; framesize:          8
; elim ap -> fp       2
; elim fp -> sp       8
; saved regs:(none)
	; start of prologue
	SUB.W	#8, R1
	; end of prologue
	MOV.W	R12, 2(R1)
	MOV.W	R13, @R1
	MOV.W	#22136, 4(R1)
	MOV.W	#4660, 6(R1)
	MOV.W	2(R1), R15
	MOV.W	4(R1), R13
	MOV.W	6(R1), R14
	MOV.W	@R1, R12
 ; 305 "toolkit/fastarithmetic.c" 1
	 mov   R12,R14 
 clr   R13 
 sub   0(R15),R13 
 subc  2(R15),R14 
 rla   R13 
 rlc   R14 
 jc    1f 
 rlc   R13 
 rlc   R14 
 rlc   R13 
 and   #0x0003,R13 
 jmp   2f 
1: 
 rlc   R13 
 rlc   R14 
 rlc   R13 
 bis   #0xFFFC,R13 
2: 
 add   0(R15),R14 
 addc  2(R15),R13 
 mov   R14,0(R15) 
 mov   R13,2(R15) 
 sub   R13,R12 

 ; 0 "" 2
	MOV.W	R12, @R1
	MOV.W	@R1, R12
	; start of epilogue
	ADD.W	#8, R1
	RET
	.size	dc_filter, .-dc_filter
	.ident	"GCC: (GNU) 4.9.1 20140707 (prerelease (msp430-14r1-364)) (GNUPro 14r1) (Based on: GCC 4.8 GDB 7.7 Binutils 2.24 Newlib 2.1)"
