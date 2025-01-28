#include "../include/oneshot.h"

bool show_steps = false;
size_t ssol = 0;

#define PRINT_STEP(base, lg) do {                               \
  if (show_steps) {                                             \
    size_t l = lg;                                              \
    if (!ssol || l >= ssol)                                     \
      message("%lu step %u ~%lu**%zu\n", seed, count, base, l); \
  }                                                             \
} while (0)

#define PRINT_STEP_INT_2()     PRINT_STEP(2,    bit_length(v_int))
#define PRINT_STEP_INT_N(base) PRINT_STEP(base, base_length(v_int, base))
#define PRINT_STEP_BIG(base)   PRINT_STEP(base, mpz_sizeinbase(v_big, base))

u64 oneshot_2(
  u64 seed,
  u64 (*memo)(u64, u64, mpz_t)
) {
  if (seed == 1)
    return 0;

  u64 v_int = seed;
  mpz_t v_big; mpz_init(v_big);
  u64 count = 0;

  goto process_int;

  process_big: ++count;

  PRINT_STEP_BIG(2);

  big_2(v_big);

  if (!mpz_fits_ulong_p(v_big))
    goto process_big;

  v_int = mpz_get_ui(v_big);

  process_int: ++count;

  PRINT_STEP_INT_2();

  if (v_int & 1) {
    if (v_int > 2642245UL) {
      // v_big = floor(v_int^(3/2))
      mpz_ui_pow_ui(v_big, v_int, 3UL);
      mpz_sqrt(v_big, v_big);

      goto process_big;
    } else {
      v_int = isqrt(v_int * v_int * v_int);
    }
  } else {
    v_int = isqrt(v_int);
  }

  if (v_int > 1)
    goto process_int;

  mpz_clear(v_big);

  return count;
}

u64 oneshot_n(
  u64 seed,
  u64 base,
  u64 (*memo)(u64, u64, mpz_t)
) {
  u64 v_int = seed;
  mpz_t v_big; mpz_init(v_big);
  u64 count = 0, additional;

  u64 past = 0;
  const u32 threshold = nthroot((u64)-1, base + 1);
  fprintf(stderr, "threshold = %u\n", threshold);

  goto process_int;

  process_big: ++count;

  PRINT_STEP_BIG(base);

  if (memo)
    if ((additional = memo(base, 0, v_big)))
      return (count - 1) + additional;

  big_n(v_big, base);

  if (!mpz_fits_ulong_p(v_big))
    goto process_big;

  v_int = mpz_get_ui(v_big);

  process_int: ++count;

  past = v_int;

  PRINT_STEP_INT_N(base);

  if (memo)
    if ((additional = memo(base, v_int, NULL)))
      return (count - 1) + additional;

  u64 r = v_int % base;
  if (v_int > threshold && r > 0) {
    u64 power = (r == base - 1) ? base + 1 : r + 1;
    mpz_ui_pow_ui(v_big, v_int, power);
    mpz_root(v_big, v_big, base);

    goto process_big;
  } else {
    u64 power = (r == base - 1) ? base + 1 : r + 1;
    v_int = nthroot(ipow(v_int, power), base);
  }

  if (v_int != past)
    goto process_int;

  mpz_clear(v_big);

  return count - 1;
}

/*
u64 oneshot(
  u64 seed,
  u64 base,
  u64 (*memo)(u64, u64, mpz_t)
) {
  return (base == 2) ? oneshot_2(seed, memo) : oneshot_n(seed, base, memo);
}
*/

#undef PRINT_STEP
#undef PRINT_STEP_INT_2
#undef PRINT_STEP_INT_N
#undef PRINT_STEP_BIG
