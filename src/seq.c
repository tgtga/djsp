#include "../include/seq.h"

bool show_steps = false;
size_t ssol = 0;

bool ending = false;
unsigned long start_value = 1, end_value = 0;

__attribute__((aligned(4))) bool
  realloc_before_up   = false, realloc_after_up   = false,
  realloc_before_down = false, realloc_after_down = false;


static void big_12(
  mpz_t v
) {
  size_t limbs = mpz_size(v);

  mpz_t root; mpz_init(root);
  {
    mp_limb_t *root_mpp = mpz_limbs_write(root, (limbs + 1) / 2);

    mpn_sqrtrem(root_mpp, NULL, mpz_limbs_read(v), limbs);

    mpz_limbs_finish(root, (limbs + 1) / 2);
  }

  mpz_clear(v);

  *v = *root;
}

static void big_32(
  mpz_t v
) {
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
}

u64 sequence(
  u64 seed
) {
  u64 v_int = seed;
  mpz_t v_big; mpz_init(v_big);
  u64 count = 0;

  goto process_int;

  process_big: ++count;

  if (show_steps) {
    size_t bl = mpz_sizeinbase(v_big, 2);
    if (!ssol || bl >= ssol)
      message("%lu step %u ~2**%zu\n", seed, count, bl);
  }

  if (mpz_odd_p(v_big)) {
    if (realloc_before_up)
      mpz_realloc2(v_big, mpz_sizeinbase(v_big, 2));

    big_32(v_big);

    if (realloc_after_up)
      mpz_realloc2(v_big, mpz_sizeinbase(v_big, 2));
  } else {
    if (realloc_before_down)
      mpz_realloc2(v_big, mpz_sizeinbase(v_big, 2));

    big_12(v_big);

    if (realloc_after_down)
      mpz_realloc2(v_big, mpz_sizeinbase(v_big, 2));
  }
  
  if (!mpz_fits_ulong_p(v_big))
    goto process_big;

  v_int = mpz_get_ui(v_big);

  process_int: ++count;

  if (show_steps) {
    size_t bl = bit_length(v_int);
    if (!ssol || bl >= ssol)
      message("%lu step %u ~2**%u\n", seed, count, bl);
  }

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

int main(void) {
  u64 n = 78901;
  show_steps = 1; ssol = 100000;
  printf("%lu: %lu\n", n, sequence(n));
}
