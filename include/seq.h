#ifndef SEQ
#define SEQ

#include <gmp.h>
#include <stdbool.h>
#include "logging.h"
#include "intmath.h"
#include "defs.h"

extern bool show_steps;
extern size_t ssol;

extern bool ending;
extern unsigned long start_value, end_value;

extern bool
  realloc_before_down, realloc_after_down,
  realloc_before_up,   realloc_after_up;

u64 sequence(u64);

#endif
