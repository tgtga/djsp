#ifndef INTMATH
#define INTMATH

#include <stdint.h>
#include <assert.h>

typedef   uint8_t     u8;
typedef   uint16_t   u16;
typedef   uint32_t   u32;
typedef   uint64_t   u64;
typedef __uint128_t u128;

int bit_length(u64);
int base_length(u64, u64);

u32 isqrt(u64);

u64 ipow(u64, u64);
u32 nthroot(u64, u64);

#endif
