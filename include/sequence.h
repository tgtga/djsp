#ifndef SEQUENCE
#define SEQUENCE

#include <gmp.h>
#include <stdbool.h>
#include <omp.h>
#include "logging.h"
#include "intmath.h"
#include "defs.h"
#include "oneshot.h"

typedef void (*hwm_callback)(u64, u64, u64);
typedef void (*memo_write_callback)(u64, u64);

void sequence          (u64, u64, u64, memo_read_callback, memo_write_callback, hwm_callback);
void sequence_2_rootopt(     u64, u64, memo_read_callback, memo_write_callback, hwm_callback);
void sequence_2_p      (     u64, u64, memo_read_callback, memo_write_callback, hwm_callback);

#endif
