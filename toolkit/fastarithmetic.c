
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

//(al)*(bh,bl) = al*bl + ( al*bh )*2^16
void mac64_16_24_fast(int64_t *z, int16_t x, int32_t y){

	__asm__ volatile(
			"__MPY=0x130 \n"
			"__OP2=0x138 \n"
			"__RESLO=0x13a \n"
			"__RESHI=0x13c \n"

			//Find out the sign of the calculation
		    " clr	R4 \n"
		    " tst	%B[y] \n"
		    " jge	mac64_16_24_1 \n"
		    // The 24 bit y is negative
		    " xor	#1,R4 \n"
		    " inv	%B[y] \n"
			" inv	%A[y] \n"
		    " add	#1,%A[y] \n"
		    " adc	%B[y] \n"
			"mac64_16_24_1: \n"
		    " tst	%[x] \n"
		    " jge	mac64_16_24_2 \n"
		    // The 16 bit x is negative
		    " xor	#1,R4 \n"
		    " inv	%[x] \n"
		    " add	#1,%[x] \n"
			"mac64_16_24_2: \n"
			" mov   %[x],&__MPY \n"
			" tst	R4 \n"
			" jeq	mac64_16_24_3 \n"
			// The answer to the real multiply will be negative, so subtract the unsigned one from the sum
			" mov.w	%B[y],&__OP2 \n"
			" mov.w	#__RESLO,R4 \n"		//Put address of RESLO in R4
			" sub.w	@R4+,%B[res] \n"	//Sub RESLO from 2 byte of z, point R4 to RESHI
			" subc.w	@R4,%C[res] \n"	//Sub RESHI from 3 byte of z
			" subc.w	#0,%D[res] \n"	//Take care of carry

		    " mov.w   %A[y],&__OP2 \n"
			" sub.w   #2,R4 \n"
			" sub.w	@R4+,%A[res] \n"
			" subc.w  @R4,%B[res] \n"
			" subc.w	#0,%C[res] \n"
			" subc.w	#0,%D[res] \n"

//			" mov	%A[y], &__OP2 \n"
//			" sub	&__RESLO, %A[res] \n"
//			" subc	&__RESHI, %B[res] \n"
//			" subc	#0, %C[res] \n"
//			" subc	#0, %D[res] \n"
//
//			//x * yh * 2^16
//			" mov	%B[y], &__OP2 \n"
//			" sub	&__RESLO, %B[res] \n"
//			" addc	&__RESHI, %C[res] \n"
//			" subc	#0, %D[res] \n"

			" jmp	done \n"

			"mac64_16_24_3: \n"
			// The answer to the real multiply will be positive, so add the unsigned one to the sum
			" mov	%A[y], &__OP2 \n"
			" add	&__RESLO, %A[res] \n"
			" addc	&__RESHI, %B[res] \n"
			" adc	%C[res] \n"
			" adc	%D[res] \n"

			//x * yh * 2^16
			" mov	%B[y], &__OP2 \n"
			" add	&__RESLO, %B[res] \n"
			" addc	&__RESHI, %C[res] \n"
			" adc	%D[res] \n"



//			//x * yl
//			" mov	%[x], &__MPY \n"
//			" mov	%A[y], &__OP2 \n"
//			" add	&__RESLO, %A[res] \n"
//			" addc	&__RESHI, %B[res] \n"
//			" adc	%C[res] \n"
//			" adc	%D[res] \n"
//
//			//x * yh * 2^16
//			" mov	%B[y], &__OP2 \n"
//			" add	&__RESLO, %B[res] \n"
//			" addc	&__RESHI, %C[res] \n"
//			" adc	%D[res] \n"
			"done: \n"

			: [res] "=m"(*z)			//Output Operands
			: [x] "r"(x), [y] "r"(y)	//Input Operands
			: "R4" 						//(Sign flag) Clobbers
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
