#include <stdint.h>

typedef   uint8_t     u8;
typedef   uint16_t   u16;
typedef   uint32_t   u32;
typedef   uint64_t   u64;
typedef __uint128_t u128;

int bit_length(
  u64 n
) {
  return 64 - __builtin_clzl(n);
}

u32 isqrt(
  u64 n
) {
  u64
    a = (u64)1 << ((bit_length(n) + 1) >> 1),
    b = (a + n / a) >> 1;

  while (b < a) {
    a = b;
    b = (a + n / a) >> 1;
  }

  return (u32)a;
}
