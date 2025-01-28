#include "../include/sequence.h"

void sequence(
  u64 base,
  u64 start, u64 end, u64 step, bool endless
) {
  u64 hwm = 0, hwm_index = 0;

  for (u64 i = start; endless || i < end; i += step) {
    u64 r = base ? oneshot_n(i, base, NULL) : oneshot_2(i, NULL);
    if (r > hwm) {
      hwm = r; ++hwm_index;

      char *bit; asprintf(&bit, base ? "_%lu" : "", base);
      message("A%s(%lu) = %lu\n", bit, hwm_index, hwm);
      free(bit);
    }
  }
}
