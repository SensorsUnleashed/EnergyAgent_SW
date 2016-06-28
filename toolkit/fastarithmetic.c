
#include "stdint.h"
//#include <msp430.h>

/*
 * x² and accumulate in z
 * Tested and found working
 * */
void sqac48_16_fast(register int16_t z[3], register int16_t x){

		__asm__ __volatile__(
				"__MPYS=0x132 \n"
				"__OP2=0x138 \n"
				"__RESLO=0x13a \n"
				"__RESHI=0x13c \n"

				//x * x
				" mov	%A[src], &__MPYS \n"		// MOV xl to MPYS
				" mov	%A[src], &__OP2 \n"		// MOV xl to OP2 (Multiplication started)
				" mov.w	#__RESLO, R4 \n"
				" add.w   @R4+,0(%[res]) \n"
				" addc.w  @R4+,2(%[res]) \n"
				" addc.w  @R4,4(%[res]) \n"

				: [res] "+r"(z)	//Output Operands
				: [src] "r"(x)		//Input Operands
				: "R4"	//Clobbers
		);
}

/*
 * x² and accumulate in z
 * Tested and found working
 * */
void sqac64_24_fast(int64_t* z, int32_t x)
{
	__asm__ __volatile__(
	"__MPY=0x130 \n"
	"__OP2=0x138 \n"
	"__RESLO=0x13a \n"

//    ; We need to do the multiply in 4 chunks. It isn't easy to
//    ; do this with signed multiplies, so we take care of the signs first
//    ; and then do 2 unsigned multiplies

    " tst.w   %B[x] \n"
    " jge     sqac64_24_1 \n"
    //; The 24 bit x is negative
    " inv.w   %B[x] \n"
    " inv.w   %A[x] \n"
    " add.w   #1,%A[x] \n"
    " addc.w  #0,%B[x] \n"
	"sqac64_24_1: \n"
    " mov.w   %B[x],&__MPY \n"
    " mov.w   %B[x],&__OP2 \n"
    " mov.w   #__RESLO,R4 \n"
    " add.w   @R4+,4(%[z]) \n"
    " addc.w  @R4,6(%[z]) \n"

    " mov.w   %A[x],&__MPY \n"
    " mov.w   %A[x],&__OP2 \n"
    " sub.w   #2,R4 \n"
    " add.w   @R4+,0(%[z]) \n"
    " addc.w  @R4,2(%[z]) \n"
    " addc.w  #0,4(%[z]) \n"
    " addc.w  #0,6(%[z]) \n"

    //; Note: It is this use of a shift which stops this routine being a full 32bit operation
    " rla.w   %B[x] \n"
    " mov.w   %B[x],&__OP2 \n"
    " sub.w   #2,R4 \n"
    " add.w   @R4+,2(%[z]) \n"
    " addc.w  @R4,4(%[z]) \n"
    " addc.w  #0,6(%[z]) \n"

	: [z] "+r"(z)	//Output Operands
	: [x] "r"(x)		//Input Operands
	: "R4"	//Clobbers
	);
}

int32_t mul48_32_16_fast(int32_t x, uint16_t y){


	__asm__ volatile(
			"__MPY=0x130 \n"
			"__MPYS=0x132 \n"
			"__MAC=0x134 \n"
			"__MACS=0x136 \n"
			"__OP2=0x138 \n"
			"__RESLO=0x13a \n"
			"__RESHI=0x13c \n"
			"__SUMEXT=0x13e \n"
			" push.w  SR \n"	//Store Status Register and Disable interrupt
			" dint \n"
			" tst.w	%B[x] \n"
			" jge	mul48_32_16_1 \n"
			//The 32 bit x is negative
			" inv.w   %A[x] \n"
			" inv.w   %B[x] \n"
			" add.w   #1,%A[x] \n"
			" addc.w  #0,%B[x] \n"
			" mov.w   %[y],&__MPY \n"
			" mov.w   %A[x],&__OP2 \n"
			" nop \n"
			" mov.w   &__RESLO,%[y] \n"
			" mov.w   &__RESHI,%A[x]\n"
			" mov.w   %B[x],&__OP2\n"
			" mov.w   #0,%B[x]\n"
			" add.w   &__RESLO,%A[x]\n"
			" addc.w  &__RESHI,%B[x]\n"
		    " pop.w   SR \n"	//Re-enable Status register to enable interrupts again
		    //Make the answer negative
			" inv.w   %[y] \n"
			" inv.w   %A[x] \n"
			" inv.w   %B[x] \n"
			" add.w   #1,%[y] \n"
			" addc.w  #0,%A[x] \n"
			" addc.w  #0,%B[x] \n"
			" jmp 	mul48_32_16_done \n"
			"mul48_32_16_1: \n"
			" mov.w   %[y],&__MPY\n"
			" mov.w   %A[x],&__OP2\n"
			" nop \n"
			" nop \n"
			" mov.w   &__RESHI,%A[x] \n"
			" mov.w   %B[x],&__OP2 \n"
			" mov.w   #0,%B[x] \n"
			" add.w   &__RESLO,%A[x]\n"
			" addc.w  &__RESHI,%B[x]\n"
			" pop.w   SR \n"	//Re-enable Status register to enable interrupts again
			"mul48_32_16_done: \n"
			: [x] "+r"(x), [y] "+r"(y)		//Output Operands
			: /*[x] "r"(x), [y] "r"(y)*/	//Input Operands
			: "R4" 							//(Sign flag) Clobbers
			);
	return x;
}

int32_t imul16(register int16_t x, register int16_t y)
{
    int32_t z;

    __asm__ (
		"__MPY=0x130 \n"
		"__MPYS=0x132 \n"
		"__MAC=0x134 \n"
		"__MACS=0x136 \n"
		"__OP2=0x138 \n"
		"__RESLO=0x13a \n"
		"__RESHI=0x13c \n"
		"__SUMEXT=0x13e \n"
        " push.w  r2 \n"
        " dint \n"
        " mov   %[x],&__MPYS \n"
        " mov   %[y],&__OP2 \n"
        " mov   &__RESHI,%B[z] \n"
        " mov   &__RESLO,%A[z] \n"
        " pop.w r2 \n"
        : [z] "=r"(z)
        : [x] "r"(x), [y] "r"(y));
    return z;
}

uint32_t umul16(register uint16_t x, register uint16_t y)
{
    int32_t z;

    __asm__ (
		"__MPY=0x130 \n"
		"__MPYS=0x132 \n"
		"__MAC=0x134 \n"
		"__MACS=0x136 \n"
		"__OP2=0x138 \n"
		"__RESLO=0x13a \n"
		"__RESHI=0x13c \n"
		"__SUMEXT=0x13e \n"
        " push.w  r2 \n"
        " dint \n"
        " mov   %[x],&__MPY \n"
        " mov   %[y],&__OP2 \n"
        " mov   &__RESHI,%B[z] \n"
        " mov   &__RESLO,%A[z] \n"
        " pop.w r2 \n"
        : [z] "=r"(z)
        : [x] "r"(x), [y] "r"(y));
    return z;
}


//
///*
// * When entering the function. arg_a is put in R12+R13, arg_b is in R14+R15
// * When returning, parameter is put in R12+R13+R14+R15
// * */
uint64_t mul32(uint32_t arg_a, uint32_t arg_b) {	//switch because this is the new mspgcc from ti

// (ah,al)*(bh,bl) = al*bl + al*bh*2^16 + ( ah*bl + ah*bh*2^16 )*2^16
//                 = al*bl + ( al*bh + ah*bl )*2^16 + ah*bh*2^32
	uint64_t ret;
	__asm__ volatile(
			"__MPY=0x130 \n"
			"__MPYS=0x132 \n"
			"__MAC=0x134 \n"
			"__MACS=0x136 \n"
			"__OP2=0x138 \n"
			"__RESLO=0x13a \n"
			"__RESHI=0x13c \n"
			"__SUMEXT=0x13e \n"

			//" push	R4 \n"		// PUSH R4 to the stack.
		    " push.w  SR \n"
		    " dint \n"

			// Fill OP1 = MPY = 0x130 with al
			" mov	R12, &__MPY \n"		// MOV al,MPY
			// Fill OP2 with bl => al * bl - the mulitplication is automatically started.
			" mov	R14, &__OP2 \n"		// MOV bl,OP2
			// Store ah in reg 4 (temp)
			" mov	R13, R4 \n"		// MOV ah,R4

			// Store low part of result from al * bl = RESLO in al
			" mov	&__RESLO, %A[res] \n"	// MOV RESLO,al

			// Store high part of result from al * bl = RESHI in ah
			" mov	&__RESHI, %B[res] \n"

			//Fill OP2 with bh => al * bh - the mulitplication is automatically started.
			" mov	R15, &__OP2 \n"		// MOV bh,OP2

			// Fill the MAC with temp (original ah)
			" mov	R4, &__MAC \n"	// MOV R4,MAC

			// Fill the OP2 with bl => ah * bl + al*bh
			" mov	R14, &__OP2 \n"		// MOV bl,OP2

			" mov	#0, R14 \n"		// MOV #0,bl

			" mov	R15, &__MPY \n"		// MOV bh,MPY

			" mov	#0, R15 \n"		// MOV #0,bh

			//" add	&__RESLO, R13 \n"		// ADD RESLO,ah
			" add	&__RESLO, %B[res] \n"
			//" addc	&__RESHI, R14 \n"	// ADDC RESHI,bl
			" addc	&__RESHI, %C[res] \n"
			//" addc	&__SUMEXT, R15 \n"	// ADDC SUMEXT,bh
			" addc	&__SUMEXT, %D[res] \n"

			" mov	R4, &__OP2 \n"		// MOV R4,OP2
			//" add	&__RESLO, R14 \n"		// ADD RESLO,bl
			" add	&__RESLO, %C[res] \n"
			//" addc	&__RESHI, R15 \n"	// ADDC RESHI,bh
			" addc	&__RESHI, %D[res] \n"
			" pop.w   SR \n"
			//" pop	R4 \n"	//);				// POP R4
			: [res] "=r" (ret)	//Output Operands	("=r" => output in registers (Default from R12)
			: 					//Input Operands
			: "R4"				//Clobbers
			);
	return ret;
}

int16_t dc_filter(int32_t *p, int16_t x)
{
    int32_t z;

    z = 0x12345678;
    __asm__ (
        " mov   %[x],%B[z] \n"
        " clr   %A[z] \n"
        " sub   0(%[p]),%A[z] \n"       // subtract filter_state from (sample << 16)
        " subc  2(%[p]),%B[z] \n"
        " rla   %A[z] \n"               // >> 14
        " rlc   %B[z] \n"
        " jc    1f \n"
        " rlc   %A[z] \n"
        " rlc   %B[z] \n"
        " rlc   %A[z] \n"
        " and   #0x0003,%A[z] \n"
        " jmp   2f \n"
        "1: \n"
        " rlc   %A[z] \n"
        " rlc   %B[z] \n"
        " rlc   %A[z] \n"
        " bis   #0xFFFC,%A[z] \n"
        "2: \n"
        " add   0(%[p]),%B[z] \n"       // + filter_state
        " addc  2(%[p]),%A[z] \n"
        " mov   %B[z],0(%[p]) \n"       // save new filter_state
        " mov   %A[z],2(%[p]) \n"
        " sub   %A[z],%[x] \n"          // sample - filter_state
        : [x] "+r"(x)
        : [p] "r"(p), [z] "r"(z));
    return x;
}

int32_t div48(int16_t x[3], int16_t y)
{
    /* Divide a 16 bit integer into a 48 bit one. Expect the answer to be no
       greater than 32 bits, so return the answer as a 32 bit integer.
       A somewhat domain specific divide operation, but pretty useful when
       handling dot products. */
    int32_t x1;
    int32_t z;

    /* Avoid any divide by zero trouble */
    if (y == 0)
        return 0;
    x1 = x[2]%y;
    x1 <<= 16;
    x1 |= (uint16_t) x[1];
    z = x1/y;
    x1 = x1%y;
    x1 <<= 16;
    x1 |= (uint16_t) x[0];
    z = (z << 16) + x1/y;
    return z;
}

uint32_t isqrt32(uint32_t h)
{
    uint32_t x;
    uint32_t y;
    int i;

    x = 0x80000000;
    y = 0;
    i = 32;
    __asm__ volatile(
        "1: \n"
        " setc \n"
        " rlc   %A[x] \n"
        " rlc   %B[x] \n"
        " sub   %A[x],%A[y] \n"
        " subc  %B[x],%B[y] \n"
        " jhs   2f \n"
        " add   %A[x],%A[y] \n"
        " addc  %B[x],%B[y] \n"
        " sub   #2,%A[x] \n"
        "2: \n"
        " inc   %A[x] \n"
        " rla   %A[h] \n"
        " rlc   %B[h] \n"
        " rlc   %A[y] \n"
        " rlc   %B[y] \n"
        " rla   %A[h] \n"
        " rlc   %B[h] \n"
        " rlc   %A[y] \n"
        " rlc   %B[y] \n"
        " dec   %[i] \n"
        " jne   1b \n"
        : [x] "+r"(x), [y] "+r"(y)
        : [h] "r"(h), [i] "r"(i)

    );

    return  x;
}

