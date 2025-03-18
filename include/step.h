#ifndef STEP
#define STEP

#include <gmp.h>
#include <stdbool.h>
#include "logging.h"
#include "intmath.h"
#include "defs.h"

extern __attribute__((aligned(4))) bool
  step_realloc_before_up,   step_realloc_after_up,
  step_realloc_before_down, step_realloc_after_down;

extern inline void step_big_2(mp_limb_t **, mp_size_t *, mp_limb_t **, mp_size_t *);
extern inline void step_big_n(mpz_t, u64);

#endif
