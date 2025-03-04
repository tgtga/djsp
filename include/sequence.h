#ifndef SEQUENCE
#define SEQUENCE

#include <gmp.h>
#include <stdbool.h>
#include "logging.h"
#include "intmath.h"
#include "defs.h"
#include "oneshot.h"

typedef void (*hwm_callback)(u64, u64, u64);

void sequence(   u64, u64, u64, u64, bool, memo_callback, hwm_callback);
void sequence_rootopt(u64, u64, u64, bool, memo_callback, hwm_callback);

#endif
