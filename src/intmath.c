#include "../include/intmath.h"

int bit_length(
  u64 n
) {
  // this SHOULD(?) be more optimal than '__builtin_ffsl(n)'
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
  // assert(n <= ((u64)1 << 61));

  u64
    f = (u64)1 << ((bit_length(n) >> 1) + 1),
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

u64 ipow_overflow(
  u64 x,
  u64 p,
  int *overflowed
) {
  u64 r = 1;

  while (p > 0) {
    if (p & 1)
      *overflowed = *overflowed | __builtin_umull_overflow(r, x, &r);

    p >>= 1;
    *overflowed = *overflowed | (__builtin_umull_overflow(x, x, &x) & !!p);
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

u64 nthroot_newton(
  u64 n,
  u64 b
) {
# define UPDATE(x) \
    ((b - 1) * (x) + n / ipow((x), b - 1)) / b

  u64
    f = (u64)1 << (bit_length(n) / b + 1),
    u = UPDATE(f);

  while (u < f) {
    f = u;
    u = UPDATE(f);
  }

# undef UPDATE

  return f;
}

u64 nthroot_linear(
  u64 n,
  u64 b
) {
  for (u64 i = 1; ; ++i) {
    int overflowed;
    u64 power = ipow_overflow(i, b, &overflowed);

    if (power >= n)
      // reached the target,
      // if the power we hit isn't right on the mark then correct the index
      return (power == n) ? i : i - 1;

    if (overflowed)
      // if the power overflowed,
      // then it must have exceeded n
      return i - 1;
  }
}

u64 nthroot(
  u64 n,
  u64 b
) {
  if (b == 0) {
    assert(n != 0);
    return 1;
  }

  if (b == 1)
    return n;

  if (b >= 64)
    return (n != 0);

  if (b == 9 || b == 12 || b >= 14)
    return nthroot_linear(n, b);

  return nthroot_newton(n, b);
}

#if 0
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

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

  /*
  int o = 0;
  ipow_safe(1 << 16, 2, &o);
  printf("%d\n", o);
  // return 0;
  */

  /*
  for (u64 b = 2; b < 16; ++b) {
    for (u64 n = 2; n; n <<= 1) {
      u64 m = n - 1;

      long double
        nf = floorl(powl((long double)n, (long double)1 / b)),
        mf = floorl(powl((long double)m, (long double)1 / b));

      int no = 0, mo = 0;
      u64
        kn = (u64)nf - (powl(nf, b) > n), tn = nthroot_newton_safe(n, b, &no),
        km = (u64)mf - (powl(mf, b) > m), tm = nthroot_newton_safe(m, b, &mo);

      if (no || kn != tn) {
        printf("%lu^(1/%lu): %lu vs %lu", n, b, kn, tn);

        if (no)
          printf(" (overflowed)");

        printf("\n");
      }

      if (mo || km != tm) {
        printf("%lu^(1/%lu): %lu vs %lu", m, b, km, tm);

        if (mo)
          printf(" (overflowed)");

        printf("\n");
      }
    }
  }
  */

  setlinebuf(stdout);

  u64 n, b;

  char *line;
  size_t len = 0;
  ssize_t nread;
  while ((nread = getline(&line, &len, stdin)) != -1) {
    char *end;
    n = strtoul(line, &end, 0);
    b = strtoul(end + 1, NULL, 0);

    printf("%lu\n", nthroot(n, b));
  }

  free(line);
}
#endif
