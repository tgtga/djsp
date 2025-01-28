#ifndef STEP
#define STEP

#include <gmp.h>
#include <stdbool.h>
#include "logging.h"
#include "intmath.h"
#include "defs.h"

extern __attribute__((aligned(4))) bool
  realloc_before_up,   realloc_after_up,
  realloc_before_down, realloc_after_down;

// FIXME: add namespace % -> step_%

extern inline void big_2(mpz_t);
extern inline void big_n(mpz_t, u64);

#endif
