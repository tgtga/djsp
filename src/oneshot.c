#include "../include/oneshot.h"

bool show_steps = false;
size_t ssol = 0;

#define PRINT_STEP(base, lg) do {                                    \
  if (show_steps) {                                                  \
    size_t l = lg;                                                   \
    if (ssol == 0 || l >= ssol)                                      \
      djsp_message("%lu step %u ~%lu**%zu\n", seed, count, base, l); \
  }                                                                  \
} while (0)

#define PRINT_STEP_INT_2()     PRINT_STEP(2,    bit_length(v_int))
#define PRINT_STEP_INT_N(base) PRINT_STEP(base, base_length(v_int, base))
#define PRINT_STEP_BIG(base)   PRINT_STEP(base, mpz_sizeinbase(v_big, base))

u64 oneshot_2(u64 seed) {
  if (seed == 1)
    return 0;

  u64 v_int = seed;
  mp_limb_t *vp = NULL; mp_size_t vl;
  u64 count = 0, additional;

  goto process_int;

  process_big: ++count;

  PRINT_STEP(2, mpn_sizeinbase(vp, vl, 2));

  step_big_2(&vp, &vl, &vp, &vl);

  mpz_t temp_ulong; temp_ulong->_mp_d = vp; temp_ulong->_mp_size = vl;
  if (!mpz_fits_ulong_p(temp_ulong))
    goto process_big;

  v_int = mpz_get_ui(temp_ulong);

  process_int: ++count;

  PRINT_STEP_INT_2();

  if (v_int & 1) {
    if (v_int > 2642245UL) {
      // v_big = floor(v_int^(3/2))
      mpz_t temp; mpz_init(temp);
      mpz_ui_pow_ui(temp, v_int, 3UL);
      mpz_sqrt(temp, temp);

      // first time through, vp will be null
      // every other free should be guaranteed safe though
      // if (vp) free(vp);
      vp = temp->_mp_d; vl = temp->_mp_size;
      while (vp[vl - 1] == 0)
        --vl;

      goto process_big;
    } else {
      v_int = isqrt(v_int * v_int * v_int);
    }
  } else {
    v_int = isqrt(v_int);
  }

  if (v_int > 1)
    goto process_int;

  free(vp);

  return count;
}

u64 oneshot_2_memo(
  u64 seed,
  memo_callback memo
) {
  if (seed == 1)
    return 0;

  u64 v_int = seed;
  mp_limb_t *vp = NULL; mp_size_t vl;
  u64 count = 0, additional;

  goto process_int;

  process_big: ++count;

  PRINT_STEP(2, mpn_sizeinbase(vp, vl, 2));

  if (memo && (additional = memo(0, vp, vl)))
		return (count - 1) + additional;

  step_big_2(&vp, &vl, &vp, &vl);

  mpz_t temp_ulong; temp_ulong->_mp_d = vp; temp_ulong->_mp_size = vl;
  if (!mpz_fits_ulong_p(temp_ulong))
    goto process_big;

  v_int = mpz_get_ui(temp_ulong);

  process_int: ++count;

  PRINT_STEP_INT_2();

  if (memo && (additional = memo(v_int, NULL, 0)))
    return (count - 1) + additional;

  if (v_int & 1) {
    if (v_int > 2642245UL) {
      // v_big = floor(v_int^(3/2))
      mpz_t temp; mpz_init(temp);
      mpz_ui_pow_ui(temp, v_int, 3UL);
      mpz_sqrt(temp, temp);

      // first time through, vp will be null
      // every other free should be guaranteed safe though
      // if (vp) free(vp);
      vp = temp->_mp_d; vl = temp->_mp_size;
      while (vp[vl - 1] == 0)
        --vl;

      goto process_big;
    } else {
      v_int = isqrt(v_int * v_int * v_int);
    }
  } else {
    v_int = isqrt(v_int);
  }

  if (v_int > 1)
    goto process_int;

  free(vp);

  return count;
}

u64 oneshot_n(
  u64 seed,
  u64 base /*,
  memo_callback memo */
) {
  u64 v_int = seed;
  mpz_t v_big; mpz_init(v_big);
  u64 count = 0, additional;

  u64 past = 0;
  const u32 threshold = nthroot((u64)-1, base + 1);
  // fprintf(stderr, "threshold = %u\n", threshold);

  goto process_int;

  process_big: ++count;

  PRINT_STEP_BIG(base);

  /*
  if (memo)
    if ((additional = memo(count, 0, v_big)))
      return (count - 1) + additional;
  */

  step_big_n(v_big, base);

  if (!mpz_fits_ulong_p(v_big))
    goto process_big;

  v_int = mpz_get_ui(v_big);

  process_int: ++count;

  past = v_int;

  PRINT_STEP_INT_N(base);

  /*
  if (memo)
    if ((additional = memo(count, v_int, NULL)))
      return (count - 1) + additional;
  */

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
