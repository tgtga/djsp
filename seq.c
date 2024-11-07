#ifndef SEQ
#define SEQ

#include <gmp.h>
#include "logging.c"
#include "intmath.c"
#include "defs.c"

int show_steps = 0;
unsigned long ssol = 0;

int ending = 0;
unsigned long start_value = 1, end_value = 0;

int
  realloc_before_even = 0,
  realloc_after_even  = 0,
  realloc_before_odd  = 0,
  realloc_after_odd   = 0;

u16 sequence(
  u64 seed
) {
  u64 v_int = seed;
  mpz_t v_big; mpz_init(v_big);
  u16 count = 0;

  while (1) {
    process_int:

    if (show_steps) {
      int bl = bit_length(v_int);
      if (!ssol || bl >= ssol)
        message("%lu step %u ~2**%u\n", seed, count, bl);
    }

    if (v_int & 1) {
      if (v_int > 2642245UL) {
        mpz_ui_pow_ui(v_big, v_int, 3UL);
        mpz_sqrt(v_big, v_big);

        ++count;

        if ((mpz_odd_p(v_big) && (mpz_cmp_ui(v_big, 2642245UL) > 0)) || !mpz_fits_ulong_p(v_big)) {
          goto process_big;
        } else {
          v_int = mpz_get_ui(v_big);

          goto process_int;
        }
      }

      if (v_int == 1)
        break;

      v_int = isqrt(v_int * v_int * v_int);
      ++count;

      goto process_int;
    } else {
      v_int = isqrt(v_int);
      ++count;

      goto process_int;
    }

    process_big:

    if (show_steps) {
      size_t bl = mpz_sizeinbase(v_big, 2);
      if (!ssol || bl >= ssol)
        message("%lu step %u ~2**%zu\n", seed, count, bl);
    }

    if (mpz_odd_p(v_big)) {
      if (realloc_before_odd)
        mpz_realloc2(v_big, mpz_sizeinbase(v_big, 2));

      mp_size_t limbs = mpz_size(v_big);



      // square = v ** 2

      mpz_t square; mpz_init(square);
      mp_limb_t *square_mpp = mpz_limbs_write(square, limbs * 2);

      mpn_sqr(square_mpp, mpz_limbs_read(v_big), limbs);

      mpz_limbs_finish(square, limbs * 2);



      // cube = square * v = v ** 3

      mpz_t cube; mpz_init(cube);
      mp_limb_t *cube_mpp = mpz_limbs_write(cube, limbs * 3);

      mpn_mul(cube_mpp, mpz_limbs_read(square), limbs * 2, mpz_limbs_read(v_big), limbs);

      mpz_clear(square);

      mpz_limbs_finish(cube, limbs * 3);



      // v = isqrt(cube) = isqrt(v ** 3) = floor(v ** 3/2)

      const mp_limb_t *cube_mpp_r = mpz_limbs_read(cube);
      mp_size_t cube_limbs = limbs * 3;

      while (cube_mpp_r[cube_limbs - 1] == 0)
        --cube_limbs;

      mp_limb_t *v_mpp = mpz_limbs_write(v_big, (cube_limbs + 1) / 2);

      mpn_sqrtrem(v_mpp, NULL, cube_mpp_r, cube_limbs);

      mpz_clear(cube);

      mpz_limbs_finish(v_big, (cube_limbs + 1) / 2);


      if (realloc_after_odd)
        mpz_realloc2(v_big, mpz_sizeinbase(v_big, 2));
    } else {
      if (realloc_before_even)
        mpz_realloc2(v_big, mpz_sizeinbase(v_big, 2));

      mp_size_t limbs = mpz_size(v_big);



      mpz_t root; mpz_init(root);
      mp_limb_t *root_mpp = mpz_limbs_write(root, (limbs + 1) / 2);

      mpn_sqrtrem(root_mpp, NULL, mpz_limbs_read(v_big), limbs);

      mpz_limbs_finish(root, (limbs + 1) / 2);



      mpz_clear(v_big);

      *v_big = *root;



      if (realloc_after_even)
        mpz_realloc2(v_big, mpz_sizeinbase(v_big, 2));
    }

    ++count;

    if ((mpz_odd_p(v_big) && (mpz_cmp_ui(v_big, 2642245UL) > 0)) || !mpz_fits_ulong_p(v_big)) {
      goto process_big;
    } else {
      v_int = mpz_get_ui(v_big);

      goto process_int;
    }
  }

  return count;
}

/*
int main(void) {
  u64 n = 78901;
  show_steps = 1; ssol = 100000;
  printf("%lu: %u\n", n, sequence(n));
}
*/

#endif
