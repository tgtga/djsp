#include "../include/step.h"

bool
  realloc_before_up   = false, realloc_after_up   = false,
  realloc_before_down = false, realloc_after_down = false;

static void big_1_2(mpz_t v) {
  if (realloc_before_down)
    mpz_realloc2(v, mpz_sizeinbase(v, 2));

  size_t limbs = mpz_size(v);

  mpz_t root; mpz_init(root);
  {
    mp_limb_t *root_mpp = mpz_limbs_write(root, (limbs + 1) / 2);

    mpn_sqrtrem(root_mpp, NULL, mpz_limbs_read(v), limbs);

    mpz_limbs_finish(root, (limbs + 1) / 2);
  }

  if (realloc_after_down)
    mpz_realloc2(root, mpz_sizeinbase(root, 2));

  mpz_clear(v);
  *v = *root;
}

static void big_3_2(mpz_t v) {
  if (realloc_before_up)
    mpz_realloc2(v, mpz_sizeinbase(v, 2));

  const size_t limbs = mpz_size(v);



  // square = v ** 2

  mpz_t square_num; mpz_init(square_num);
  {
    mp_limb_t *square_raw = mpz_limbs_write(square_num, limbs * 2);

    mpn_sqr(square_raw, mpz_limbs_read(v), limbs);

    mpz_limbs_finish(square_num, limbs * 2);
  }



  // cube = square * v = v ** 3

  mpz_t cube_num; mpz_init(cube_num);
  {
    mp_limb_t *cube_raw = mpz_limbs_write(cube_num, limbs * 3);

    mpn_mul(cube_raw, mpz_limbs_read(square_num), limbs * 2, mpz_limbs_read(v), limbs);

    mpz_limbs_finish(cube_num, limbs * 3);
  }

  mpz_clear(square_num);



  // v = isqrt(cube) = isqrt(v ** 3) = floor(v ** 3/2)

  {
    const mp_limb_t *cube_raw_r = mpz_limbs_read(cube_num);

    // mpn_sqrtrem requires the top limb to be nonzero,
    // so advance 'cube_limbs' down until a nonzero limb is found
    size_t cube_limbs = limbs * 3;
    while (cube_raw_r[cube_limbs - 1] == 0)
      --cube_limbs;

    mp_limb_t *v_mpp = mpz_limbs_write(v, (cube_limbs + 1) / 2);

    mpn_sqrtrem(v_mpp, NULL, cube_raw_r, cube_limbs);

    mpz_limbs_finish(v, (cube_limbs + 1) / 2);
  }

  mpz_clear(cube_num);

  if (realloc_after_up)
    mpz_realloc2(v, mpz_sizeinbase(v, 2));
}

extern inline void big_2(mpz_t v) {
  if (mpz_odd_p(v))
    big_3_2(v);
  else
    big_1_2(v);
}



static void big_n_n(
  mpz_t v,
  u64 base,
  u64 power
) {
  mpz_pow_ui(v, v, power);
  mpz_root(v, v, base);
}

#define N_STEP(direction, power) do {         \
  if (realloc_before_ ## direction)           \
    mpz_realloc2(v, mpz_sizeinbase(v, base)); \
                                              \
  big_n_n(v, base, (power));                  \
                                              \
  if (realloc_before_ ## direction)           \
    mpz_realloc2(v, mpz_sizeinbase(v, base)); \
} while (0)

extern inline void big_n(
  mpz_t v,
  u64 base
) {
  u64 remainder = mpz_fdiv_ui(v, base);
  if (remainder == base - 1)
    N_STEP(up, base + 1);
  else
    N_STEP(up, remainder + 1);
}

#undef N_STEP
