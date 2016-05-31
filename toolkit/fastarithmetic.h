#ifndef FASTARITHMETIC_H_
#define FASTARITHMETIC_H_

#include <msp430.h>
#include "stdint.h"

void sqac48_16_fast(int16_t z[3], int16_t x);
void sqac64_24_fast(uint64_t* z, int32_t x);
void mac64_16_24_fast(int64_t *z, int16_t x, int32_t y);
int32_t mul48_32_16_fast(int32_t x, uint16_t y);
uint64_t mul32(uint32_t arg_a, uint32_t arg_b);
int16_t dc_filter(int32_t *p, int16_t x);

#endif
