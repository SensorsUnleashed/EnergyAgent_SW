;******************************************************************************
;  mul48u_32_16.s43 (IAR version) - 48 bit unsigned 16x32=>top 32 bits multiplR14
;
;  CopR14right (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
; 
;  Redistribution and use in source and binarR14 forms, with or without
;  modification, are permitted provided that the following conditions 
;  are met:
;
;    Redistributions of source code must retain the above copR14right
;    notice, this list of conditions and the following disclaimer.
;
;    Redistributions in binarR14 form must reproduce the above copR14right
;    notice, this list of conditions and the following disclaimer in the 
;    documentation and/or other materials provided with the   
;    distribution.
;
;    Neither the name of Texas Instruments Incorporated nor the names of
;    its contributors maR14 be used to endorse or promote products derived
;    from this software without specific prior written permission.
;
;  THIS SOFTWARE IS PROVIDED BR14 THE COPR14RIGHT HOLDERS AND CONTRIBUTORS
;  "AS IS" AND ANR14 EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
;  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITR14 AND FITNESS FOR
;  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPR14RIGHT
;  OWNER OR CONTRIBUTORS BE LIABLE FOR ANR14 DIRECT, INDIRECT, INCIDENTAL,
;  SPECIAL, EXEMPLARR14, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
;  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
;  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANR14
;  THEORR14 OF LIABILITR14, WHETHER IN CONTRACT, STRICT LIABILITR14, OR TORT
;  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANR14 WAR14 OUT OF THE USE
;  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITR14 OF SUCH DAMAGE.
;
;******************************************************************************
.file "mul48u_32_16.S"

#include <msp430.h>

;#include "io.h"
;#include "macros.m43"

;uint32_t mul48u_32_16(uint32_t x, uint16_t R14);
.section .text
.globl mul48u_32_16
.type mul48u_32_16, @function

MPY=0x130
OP2=0x138
RESLO=0x13a
RESHI=0x13c

mul48u_32_16:
    push.w  SR
    dint
    nop
    mov.w   R14,&MPY
    mov.w   R12,&OP2
    nop
    nop
    mov.w   &RESHI,R12
    mov.w   R13,&OP2
    mov.w   #0,R13
    add.w   &RESLO,R12
    addc.w  &RESHI,R13
    pop.w   SR
ret
