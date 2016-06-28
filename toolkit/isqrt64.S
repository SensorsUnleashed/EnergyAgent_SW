/*
 * isqrt64.s
 *
 *  Created on: 09/06/2016
 *      Author: omn
 * Ajusted version from the IAR version of the TI emeter-toolkit
 */

.file "isqrt64.S"

#include <msp430.h>

;uint64_t isqrt64(uint64_t h)
.section .text
.globl isqrt64
.type isqrt64, @function

isqrt64:

    ;The answer is calculated as a 64 bit value, where the last
    ;32 bits are fractional.
    ;Calling with negative numbers is not a good idea :-)

    push.w  R4
    push.w  R5
    push.w  R6
    push.w  R7
    push.w  R8
    push.w  R9
    push.w  R10
    push.w  R11

    ; Use a variable on the stack as the loop counter
    mov.w   #32,R11
    push.w  R11

    mov.w   #0,R8
    mov.w   #0,R9
    mov.w   #0,R10
    mov.w   #0,R11
    mov.w   #0,R4
    mov.w   #0,R5
    mov.w   #0,R6
    mov.w   #0,R7
isqrt64_1:
    setc
    rlc.w   R8
    rlc.w   R9
    rlc.w   R10
    rlc.w   R11
    sub.w   R8,R4
    subc.w  R9,R5
    subc.w  R10,R6
    subc.w  R11,R7
    jhs     isqrt64_2
    add.w   R8,R4
    addc.w  R9,R5
    addc.w  R10,R6
    addc.w  R11,R7
    sub.w   #2,R8
isqrt64_2:
    inc.w   R8
    ;
    rla.w   R12
    rlc.w   R13
    rlc.w   R14
    rlc.w   R15
    rlc.w   R4
    rlc.w   R5
    rlc.w   R6
    rlc.w   R7
    ;
    rla.w   R12
    rlc.w   R13
    rlc.w   R14
    rlc.w   R15
    rlc.w   R4
    rlc.w   R5
    rlc.w   R6
    rlc.w   R7
    ;
    dec     0(sp)
    jne     isqrt64_1

    mov.w   #32,0(sp)
isqrt64_3:
    setc
    rlc.w   R8
    rlc.w   R9
    rlc.w   R10
    rlc.w   R11
    sub.w   R8,R4
    subc.w  R9,R5
    subc.w  R10,R6
    subc.w  R11,R7
    jhs     isqrt64_4
    add.w   R8,R4
    addc.w  R9,R5
    addc.w  R10,R6
    addc.w  R11,R7
    sub.w   #2,R8
isqrt64_4:
    inc.w   R8
    ;
    rla.w   R4
    rlc.w   R5
    rlc.w   R6
    rlc.w   R7
    ;
    rla.w   R4
    rlc.w   R5
    rlc.w   R6
    rlc.w   R7
    ;
    dec     0(sp)
    jne     isqrt64_3

    mov.w   R8,R12
    mov.w   R9,R13
    mov.w   R10,R14
    mov.w   R11,R15

    pop.w   R11

    pop.w   R11
    pop.w   R10
    pop.w   R9
    pop.w   R8
    pop.w   R7
    pop.w   R6
    pop.w   R5
    pop.w   R4

ret

