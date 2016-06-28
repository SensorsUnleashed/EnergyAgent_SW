;******************************************************************************
;  mul64u_32_32.S - 64 bit unsigned 32x32
;
;******************************************************************************
.file "mul64u_32_32.S"

#include <msp430.h>


;uint64_t mul64u_32_32(uint32_t x, uint32_t y);
.section .text
.globl mul64u_32_32
.type mul64u_32_32, @function

__MPY=0x130
__MPYS=0x132
__MAC=0x134
__MACS=0x136
__OP2=0x138
__RESLO=0x13a
__RESHI=0x13c
__SUMEXT=0x13e

mul64u_32_32:
    push.w  SR
    dint
	push	R4

	push	R5
	push	R6
	push	R7
	push	R8

	mov.w	#0, R8

	;(xl * yl)
	mov	R12, &__MPY		;xl
	mov	R14, &__OP2		;yl

	mov	R13, R4
	mov	&__RESLO, R5
	mov	&__RESHI, R6
	mov &__SUMEXT, R7

	;(xl * yh + xh*yl) << 16
	mov	R15, &__OP2
	mov	R4, &__MAC
	mov	R14, &__OP2
	mov	#0, R14

	;(yh * xh) << 32
	mov	R15, &__MPY
	mov	#0, R15

	add	 &__RESLO,  R6
	addc &__RESHI,  R7
	addc &__SUMEXT, R8

	mov	R4, &__OP2
	add	&__RESLO, R7
	addc	&__RESHI, R8

	mov.w	R5, R12
	mov.w	R6, R13
	mov.w	R7, R14
	mov.w	R8, R15

	pop.w	R4
	pop.w	R5
	pop.w	R6
	pop.w	R7
	pop.w	R8

    pop.w   SR
ret

