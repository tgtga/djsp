#ifndef ONESHOT
#define ONESHOT

#include <gmp.h>
#include <stdbool.h>
#include "logging.h"
#include "intmath.h"
#include "defs.h"
#include "step.h"

extern bool show_steps;
extern size_t ssol;

//                           steps int  big
typedef u64 (*memo_callback)(u64,  u64, mpz_t);

u64 oneshot_2(u64);
u64 oneshot_n(u64, u64);

#endif
