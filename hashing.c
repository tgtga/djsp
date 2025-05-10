#include "include/step.h"

u64 run(
  u64 hash(mp_limb_t *, mp_size_t, mp_limb_t *, mp_size_t),
  u64 seed
) {
  // just allocate a shitton, no lengths over ~500 have been found
  u64 *hs = malloc(sizeof(*hs) * 1024);
  size_t len = 0;

  mpz_t v_init; mpz_init_set_ui(v_init, seed);
  mp_limb_t *vp = v_init->_mp_d; mp_size_t vl = v_init->_mp_size;

  mpz_t p_init; mpz_init_set_ui(p_init, 0);
  mp_limb_t *pp = p_init->_mp_d; mp_size_t pl = p_init->_mp_size;

  while (!(vl <= 1 && *vp == 1)) {
    hs[len++] = hash(vp, vl, pp, pl);

    free(pp);
    pp = malloc(sizeof(*pp) * vl);
    memcpy(pp, vp, sizeof(*pp) * vl);
    pl = vl;

    step_big_2(&vp, &vl, &vp, &vl);
    while (vp[vl - 1] == 0)
      --vl;
  }

  free(pp); free(vp);

  u64 count = 0;
  for (size_t c = 0; c < len; ++c) {
    for (size_t n = c + 1; n < len; ++n)
      if (hs[c] == hs[n])
        goto dup;

    ++count;
    continue;

    dup: ;
  }

  return len - count;
}

u64 hash_nothing(
  mp_limb_t *cp, mp_size_t cl,
  mp_limb_t *pp, mp_size_t pl
) {
  return 0;
}

u64 hash_stacked(
  mp_limb_t *cp, mp_size_t cl,
  mp_limb_t *pp, mp_size_t pl
) {
  return ((u64)(u32)(*cp) << 32) | (u32)(*pp);
}

static const int slice_size = 16;
u64 hash_slice(
  mp_limb_t *cp, mp_size_t cl,
  mp_limb_t *pp, mp_size_t pl
) {
  u64
    high = *cp & ((1 << slice_size) - 1),
    low  = *pp & ((1 << slice_size) - 1);

  return (high << slice_size) | low;
}

u64 hash_slicediff(
  mp_limb_t *cp, mp_size_t cl,
  mp_limb_t *pp, mp_size_t pl
) {
  u64
    high = *cp & ((1 << slice_size) - 1),
    low  = *pp & ((1 << slice_size) - 1),
    diff = (high - low) & ((1 << slice_size) - 1);

  return (high << slice_size) | diff;
}

static const int stagger_size = 1;
u64 hash_staggered_slicediff(
  mp_limb_t *cp, mp_size_t cl,
  mp_limb_t *pp, mp_size_t pl
) {
  static const int smaller_slice_size = slice_size - stagger_size;

  u64
    high = *cp & ((1 << slice_size) - 1),
    low  = *pp & ((1 << smaller_slice_size) - 1),
    diff = (high - low) & ((1 << slice_size) - 1);

  return (high << slice_size) | low;
}

int main(void) {
  step_realloc_after_down = step_realloc_after_up = step_realloc_before_down = step_realloc_before_up = 1;

  for (u64 r, k = 1; ; ++k)
    if ((r = run(hash_slice, k)) > 0)
      printf("%lu: %lu\n", k, r);
}
