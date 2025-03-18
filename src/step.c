#include "../include/step.h"

bool
  step_realloc_before_up   = false, step_realloc_after_up   = false,
  step_realloc_before_down = false, step_realloc_after_down = false;

static void step_big_1_2(
  mp_limb_t **out_p, mp_size_t *out_limbs_p,
  mp_limb_t **x_p,   mp_size_t *limbs_p
) {
  mp_limb_t *x = *x_p; mp_size_t limbs = *limbs_p;

  if (step_realloc_before_down)
    while (x[limbs - 1] == 0)
      --limbs;
  
  mp_size_t root_limbs = (limbs + 1) >> 1;
  mp_limb_t *root = malloc(sizeof(*root) * root_limbs);

  mpn_sqrtrem(root, NULL, x, limbs);
  free(*x_p);

  if (step_realloc_after_down)
    while (root[root_limbs - 1] == 0)
      --root_limbs;

  *out_p = root; *out_limbs_p = root_limbs;
}

static void step_big_3_2(
  mp_limb_t **out_p, mp_size_t *out_limbs_p,
  mp_limb_t **x_p,   mp_size_t *limbs_p
) {
  mp_limb_t *x = *x_p; mp_size_t limbs = *limbs_p;

  if (step_realloc_before_up)
    while (x[limbs - 1] == 0)
      --limbs;
  
  // square = v ** 2
  mp_size_t square_limbs = limbs * 2;
  mp_limb_t *square = malloc(sizeof(*square) * square_limbs);

  mpn_sqr(square, x, limbs);

  // cube = square * v = v ** 3
  mp_size_t cube_limbs = limbs * 3;
  mp_limb_t *cube = malloc(sizeof(*cube) * cube_limbs);

  mpn_mul(cube, square, limbs * 2, x, limbs);
  free(*x_p);
  free(square);

  mp_size_t reduction = 0;
  while (cube[cube_limbs - 1 - reduction] == 0)
    ++reduction;
  cube_limbs -= reduction;
  // if (reduction)
  //   fprintf(stderr, "in big_3_2, reduced cube_limbs by %lu\n", reduction);

  // v = isqrt(cube)

  mp_size_t out_limbs = (cube_limbs + 1) >> 1;
  mp_limb_t *out = malloc(sizeof(*out) * out_limbs);

  mpn_sqrtrem(out, NULL, cube, cube_limbs);
  free(cube);

  if (step_realloc_after_up)
    while (out[out_limbs - 1] == 0)
      --out_limbs;

  *out_p = out; *out_limbs_p = out_limbs;
}

extern inline void step_big_2(
  mp_limb_t **out_p, mp_size_t *out_limbs_p,
  mp_limb_t **x_p,   mp_size_t *limbs_p
) {
  mp_limb_t *x = *x_p; mp_size_t limbs = *limbs_p;

  // if (mpz_odd_p(MPZ_ROINIT_N(x, limbs)))
  if ((limbs != 0) & *x)
    step_big_3_2(out_p, out_limbs_p, x_p, limbs_p);
  else
    step_big_1_2(out_p, out_limbs_p, x_p, limbs_p);
}



static void step_big_n_n(
  mpz_t v,
  u64 base,
  u64 power
) {
  mpz_pow_ui(v, v, power);
  mpz_root(v, v, base);
}

#define N_STEP(direction, power) do {         \
  if (step_realloc_before_ ## direction)           \
    mpz_realloc2(v, mpz_sizeinbase(v, base)); \
                                              \
  step_big_n_n(v, base, (power));                  \
                                              \
  if (step_realloc_before_ ## direction)           \
    mpz_realloc2(v, mpz_sizeinbase(v, base)); \
} while (0)

extern inline void step_big_n(
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
