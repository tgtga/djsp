#include "../include/sequence.h"

void sequence(
  u64 base,
  u64 start, u64 end, u64 step, bool endless,
  memo_callback memo,
  hwm_callback found_hwm
) {
  u64 hwm = 0, hwm_index = 0;

  for (u64 i = start; endless || i < end; i += step) {
    u64 r = base ? oneshot_n(i, base, memo) : oneshot_2(i, memo);
    if (r > hwm) {
      hwm = r; ++hwm_index;
      if (found_hwm) found_hwm(hwm_index, hwm, i);

      /*
      char *bit; asprintf(&bit, base ? "_%lu" : "", base);
      message("A%s(%lu) @ %lu = %lu\n", bit, hwm_index, i, hwm);
      free(bit);
      */
    }
  }
}

void sequence_rootopt(
  u64 start, u64 end, u64 step, bool endless,
  memo_callback memo,
  hwm_callback found_hwm
) {
  u64 hwm = 6, hwm_index = 2;

  if (found_hwm) found_hwm(1, 1, 2); // message("A(1) @ 2 = 1\n");
  if (found_hwm) found_hwm(2, 6, 3); // message("A(2) @ 3 = 6\n");
  
  u64 n = 4;
  for (u64 stride = 2; ; ++stride) {
    u64 lower, upper, even, r;
    lower = n + !(n & 1); even = n + (n & 1);
    n += 2 * stride + 1;
    upper = n - ((n & 1) + 1);
    // printf("%lu -> %lu, condensing evens into %lu\n", lower, upper, even);

    r = oneshot_2(even, memo);
    if (r > hwm) {
      hwm = r; ++hwm_index;
      if (found_hwm) found_hwm(hwm_index, hwm, even); // message("A(%lu) @ %lu = %lu\n", hwm_index, even, hwm);
    }

    for (u64 i = lower; i <= upper; i += 2) {
      if (i > end) return;

      r = oneshot_2(i, memo);
      if (r > hwm) {
        hwm = r; ++hwm_index;
        if (found_hwm) found_hwm(hwm_index, hwm, i); // message("A(%lu) @ %lu = %lu\n", hwm_index, i, hwm);
      }
    }
  }
}
