#include <stdio.h>
#include <stdlib.h>
#include <immintrin.h>
#include <gmp.h>

static const size_t firstMallocSize = 1UL << 24;

#if 0
unsigned long isqrt(
  const unsigned long n
) {
  /*
  int shift = 64 - __builtin_clzll(n);
  shift += shift & 1;

  unsigned long result = 0;

  do {
    shift -= 2;
    result = (result << 1) | 1;
    result ^= result * result > (n >> shift);
  } while (shift);

  return result;
  */

  /*
  unsigned long result = 0;

# define iter(shift) do {                     \
    result = (result << 1) | 1;               \
    result ^= result * result > (n >> shift); \
  } while (0)

  iter(62);
  iter(60);
  iter(58);
  iter(56);
  iter(54);
  iter(52);
  iter(50);
  iter(48);
  iter(46);
  iter(44);
  iter(42);
  iter(40);
  iter(38);
  iter(36);
  iter(34);
  iter(32);
  iter(30);
  iter(28);
  iter(26);
  iter(24);
  iter(22);
  iter(20);
  iter(18);
  iter(16);
  iter(14);
  iter(12);
  iter(10);
  iter( 8);
  iter( 6);
  iter( 4);
  iter( 2);
  iter( 0);

# undef iter

  return result;
  */
  /*
  unsigned long result = 0;
  int shift = 64 - __builtin_clzll(n);
  shift += shift & 1;

# define DO(where)                                  \
    case (where):                                   \
      result = (result << 1) | 1;                   \
      result ^= (result * result) > (n >> (where));

  switch (shift) {
    case 64:
    DO(62) DO(60) DO(58) DO(56) DO(54) DO(52) DO(50) DO(48)
    DO(46) DO(44) DO(42) DO(40) DO(38) DO(36) DO(34) DO(32)
    DO(30) DO(28) DO(26) DO(24) DO(22) DO(20) DO(18) DO(16)
    DO(14) DO(12) DO(10) DO( 8) DO( 6) DO( 4) DO( 2) DO( 0)
  }

# undef DO

  return result;
  */
  unsigned long result = 0;
  int shift = 64 - __builtin_clzll(n);
  shift += shift & 1;

  static void *table[] = {
    &&j64,
    &&j62, &&j60, &&j58, &&j56, &&j54, &&j52, &&j50, &&j48,
    &&j46, &&j44, &&j42, &&j40, &&j38, &&j36, &&j34, &&j32,
    &&j30, &&j28, &&j26, &&j24, &&j22, &&j20, &&j18, &&j16,
    &&j14, &&j12, &&j10, &&j8,  &&j6,  &&j4,  &&j2,  &&j0
  };

  goto *table[shift];

  j64:
# define DO(where)                                  \
    j##where:                                       \
      result = (result << 1) | 1;                   \
      result ^= (result * result) > (n >> (where));
  DO(62) DO(60) DO(58) DO(56) DO(54) DO(52) DO(50) DO(48)
  DO(46) DO(44) DO(42) DO(40) DO(38) DO(36) DO(34) DO(32)
  DO(30) DO(28) DO(26) DO(24) DO(22) DO(20) DO(18) DO(16)
  DO(14) DO(12) DO(10) DO( 8) DO( 6) DO( 4) DO( 2) DO( 0)
# undef DO

  return result;
}
#endif

/*
static inline unsigned long isqrt(
  const unsigned long n
) {
  unsigned long result = 0;
  int shift = 63 - __builtin_clzll(n);

  static void *table[65] = {
           &&j0,  &&j2,  &&j2,  &&j4,  &&j4,  &&j6,  &&j6,
    &&j8,  &&j8,  &&j10, &&j10, &&j12, &&j12, &&j14, &&j14,
    &&j16, &&j16, &&j18, &&j18, &&j20, &&j20, &&j22, &&j22,
    &&j24, &&j24, &&j26, &&j26, &&j28, &&j28, &&j30, &&j30,
    &&j32, &&j32, &&j34, &&j34, &&j36, &&j36, &&j38, &&j38,
    &&j40, &&j40, &&j42, &&j42, &&j44, &&j44, &&j46, &&j46,
    &&j48, &&j48, &&j50, &&j50, &&j52, &&j52, &&j54, &&j54,
    &&j56, &&j56, &&j58, &&j58, &&j60, &&j60, &&j62, &&j62,
    &&j64, &&j64
  };

  goto *table[shift];

  j64:
# define DO(where)                                  \
    j##where:                                       \
      result = (result << 1) | 1;                   \
      result ^= (result * result) > (n >> (where));
  DO(62) DO(60) DO(58) DO(56) DO(54) DO(52) DO(50) DO(48)
  DO(46) DO(44) DO(42) DO(40) DO(38) DO(36) DO(34) DO(32)
  DO(30) DO(28) DO(26) DO(24) DO(22) DO(20) DO(18) DO(16)
  DO(14) DO(12) DO(10) DO( 8) DO( 6) DO( 4) DO( 2) DO( 0)
# undef DO

  return result;
}
*/

int main(void) {
  mpz_t mpzv;
  mpz_init(mpzv);

  unsigned short *predet = malloc(sizeof(*predet) * firstMallocSize);
  size_t predetAllocated = firstMallocSize;
  predet[0] = 0;
  predet[1] = 0;

  for (
    unsigned long
      gotEven = 0,
      max     = 0,
      n       = 2,
      nsq     = 1,
      nsv     = (nsq + 1) * (nsq + 1);
    ; /* __builtin_expect(n < 2500000, 1); */
    ++n
  ) {
    unsigned long count = 0, intv = n, odd;

    if (n == nsv) {
      ++nsq;
      nsv += 2 * nsq + 1; // advance to the next square
      gotEven = 0;
    }

    odd = n & 1;

    if (__builtin_expect(gotEven, 1) && !odd) {
      unsigned long square = nsq * nsq;
      square += square & 1;

      count = predet[square];

      goto done;
    }

    gotEven = gotEven || !odd;

    while (__builtin_expect(intv >= n, 1)) {
      if (odd && intv > 2642245) {
        mpz_set_ui(mpzv, intv);

        mpz_pow_ui(mpzv, mpzv, 3);
        mpz_sqrt(mpzv, mpzv);
        odd = mpz_odd_p(mpzv);

        ++count;

        while (!mpz_fits_ulong_p(mpzv) || odd) {
          if (odd)
            mpz_pow_ui(mpzv, mpzv, 3);
          mpz_sqrt(mpzv, mpzv);

          odd = mpz_odd_p(mpzv);

          ++count;
        }

        intv = mpz_get_ui(mpzv);
      }

      if (odd)
        intv = intv * intv * intv;

      int half = (63 - __builtin_clzll(intv)) / 2;
      unsigned long l = 1UL << half, r = 1UL << (half + 1);

      do {
        unsigned long m = r / 2;

        if (intv >= (l + m) * (l + m))
          l += m;

        r -= m;
      } while (r > 1);

      intv = l;

      ++count;

      odd = intv & 1;
    }

    count += predet[intv];

    // printf(" %lu\n", count);

    if (count > max) {
      printf("%lu %lu\n", n, count);
      max = count;
    }

    done: ;

    if (__builtin_expect(predetAllocated <= n, 0)) {
      predetAllocated = predetAllocated * 3 / 2;
      predet = realloc(predet, sizeof(*predet) * predetAllocated);
    }
    predet[n] = count;
  }

  free(predet);
  mpz_clear(mpzv);
}

