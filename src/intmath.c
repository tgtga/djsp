#include "../include/intmath.h"

int bit_length(
  u64 n
) {
  return 64 - __builtin_clzl(n);
}

int base_length(
  u64 n,
  u64 base
) {
  int count = 0;
  for (; n; n /= base, ++count) ;
  return count;
}



u32 isqrt(
  u64 n
) {
  u64
    // remove bl <<+ 1>>?
    f = (u64)1 << ((bit_length(n) + 1) >> 1),
    u = (f + n / f) >> 1;

  while (u < f) {
    f = u;
    u = (f + n / f) >> 1;
  }

  return (u32)f;
}

u64 ipow(
  u64 x,
  u64 p
) {
  if (p == 0) {
    assert(x != 0);
    return 0;
  }
  if (p == 1)
    return x;

  u64 r = 1;

  while (p > 0) {
    if (p & 1)
      r *= x;

    x *= x;
    p >>= 1;
  }
  
  return r;
}

// base-b heron's method derivation:
// (newton's method) f(x) = x^b - n, x_i+1 = x_i - f(x_i) / f'(x_i)
// x_i+1 = x_i - (x^b - n) / (b * x_i^(b - 1))
// x_i+1 = x_i - (1/b) * (x_i^(b - 1) * (x_i - n / x_i^(b - 1))) / x_i^(b - 1)
// x_i+1 = x_i - (1/b) * (x_i - n / x_i^(b - 1))
// x_i+1 = (b * x_i) / b - (x_i - n / x_i^(b - 1)) / b
// x_i+1 = (b * x_i - x_i + n / x_i^(b - 1)) / b
// x_i+1 = ((b - 1) * x_i + n / x_i^(b - 1)) / b

u32 nthroot(
  u64 n,
  u64 b
) {
  assert(b != 0);

  if (b == 1)
    return n;

# define UPDATE(x) (((b - 1) * (x) + n / ipow((x), b - 1)) / b)

  u64
    f = (u64)1 << (bit_length(n) / b + 1),
    u = UPDATE(f);

  while (u < f) {
    f = u;
    u = UPDATE(f);
  }

  return (u32)f;
}

#if 0
#include <stdio.h>

int main(void) {
  /*
  for (u64 b = 2; ; ++b) {
    fprintf(stderr, "on %lu...\n", b);
    for (u64 i = 1; i < ((u64)1 << 24); ++i) {
      if (ipow(nthroot(i, b), b) > i) {
        printf("%lu %lu!\n", i, b);
        break;
      }
    }
  }
  */

  u64 n = 10, b = 3;
  printf("%lu^(1/%lu) = %lu\n", n, b, nthroot(n, b));
}
#endif
