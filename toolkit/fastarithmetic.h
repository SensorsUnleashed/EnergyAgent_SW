#ifndef FASTARITHMETIC_H_
#define FASTARITHMETIC_H_

#include <msp430.h>
#include "stdint.h"

void sqac48_16_fast(int16_t z[3], int16_t x);
void sqac64_24_fast(int64_t* z, int32_t x);
int32_t mul48_32_16_fast(int32_t x, uint16_t y);
int32_t imul16(int16_t x, int16_t y);
uint32_t umul16(uint16_t x, uint16_t y);

int16_t dc_filter(int32_t *p, int16_t x);

int32_t div48(int16_t x[3], int16_t y);
uint32_t isqrt32(uint32_t h);

uint64_t mul32(uint32_t arg_a, uint32_t arg_b);

//Assembly .S files
uint64_t isqrt64(uint64_t h);
uint32_t mul48u_32_16(uint32_t x, uint16_t y);
uint64_t mul64u_32_32(uint32_t x, uint32_t y);
int64_t mul64i_48_32(int64_t x, uint32_t y);
void mac64_16_24(int64_t *z, int16_t x, int32_t y);
#endif
