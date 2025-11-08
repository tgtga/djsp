#include "../include/sequence.h"

void sequence(
	u64 base,
  u64 start, u64 end,
  memo_read_callback memo_read, memo_write_callback memo_write,
  hwm_callback found_hwm
) {
	(void)memo_read; (void)memo_write;

  u64 hwm = 0, hwm_index = 0;

  for (u64 i = start; i < end; ++i) {
    u64 r = base ? oneshot_n(i, base) : oneshot_2(i);
    if (r > hwm) {
      hwm = r; ++hwm_index;
      if (found_hwm) found_hwm(hwm_index, hwm, i);

      /*
      char *bit; asprintf(&bit, base ? "_%lu" : "", base);
      djsp_message("A%s(%lu) @ %lu = %lu\n", bit, hwm_index, i, hwm);
      free(bit);
      */
    }
  }
}

void sequence_2_rootopt(
  u64 start, u64 end,
  memo_read_callback memo_read, memo_write_callback memo_write,
  hwm_callback found_hwm
) {
	(void)memo_read; (void)memo_write;

	fprintf(stderr, "sequence_2_rootopt with %lu, %lu, %p, %p, %p\n", start, end, memo_read, memo_write, found_hwm);

  u64 hwm = 6, hwm_index = 2;

  if (found_hwm) found_hwm(1, 1, 2); // djsp_message("A(1) @ 2 = 1\n");
  if (found_hwm) found_hwm(2, 6, 3); // djsp_message("A(2) @ 3 = 6\n");

  u64 n = 4;
  for (u64 stride = 2; ; ++stride) {
    u64 lower, upper, even, r;
    lower = n + !(n & 1); even = n + (n & 1);
    n += 2 * stride + 1;
    upper = n - ((n & 1) + 1);
    // printf("%lu -> %lu, condensing evens into %lu\n", lower, upper, even);

    r = oneshot_2(even);
    if (r > hwm) {
      hwm = r; ++hwm_index;
      if (found_hwm) found_hwm(hwm_index, hwm, even); // djsp_message("A(%lu) @ %lu = %lu\n", hwm_index, even, hwm);
    }

    for (u64 i = lower; i <= upper; i += 2) {
      if (i > end) return;

      r = oneshot_2(i);
      if (r > hwm) {
        hwm = r; ++hwm_index;
        if (found_hwm) found_hwm(hwm_index, hwm, i); // djsp_message("A(%lu) @ %lu = %lu\n", hwm_index, i, hwm);
      }
    }
  }
}

typedef struct {
	u64 what, where;
} hwm_result;

void sequence_2_p(
  u64 start, u64 end,
  memo_read_callback memo_read, memo_write_callback memo_write,
  hwm_callback found_hwm
) {
	fprintf(stderr, "sequence_2_p with %lu, %lu, %p, %p, %p\n", start, end, memo_read, memo_write, found_hwm);

  u64 hwm = 6, hwm_index = 2;

  // strides is always a high estimate
  // worst case scenario more values are computed than what was asked for
  u64 strides = isqrt(end); // u64 n = 4;

	hwm_result *finished_strides = malloc(sizeof(*finished_strides) * (strides + 1));
	unsigned char *done = calloc(strides + 1, sizeof(*done)); done[0] = done[1] = done[2] = 1;
	size_t watching = 3;

  djsp_message("A(1) @ 2 = 1\n"); if (found_hwm) found_hwm(1, 1, 2);
  djsp_message("A(2) @ 3 = 6\n"); if (found_hwm) found_hwm(2, 6, 3);

# pragma omp parallel for
  for (u64 stride = 3; stride <= strides; ++stride) {
// #		ifdef TIME
// 	  	double wtime = omp_get_wtime();
// #		endif
		u64
			sqr = stride * stride, next = (stride + 1) * (stride + 1),
			even = sqr + (stride & 1),
			lower = sqr + !(stride & 1),
			upper = next - ((next & 1) + 1),
			r;
    // fprintf(stderr, "stride #%lu: %lu -> %lu, condensing evens into %lu\n", stride, lower, upper, even);

		if (lower > end)
	    fprintf(stderr, "OVERSHOT! %lu -> %lu, condensing evens into %lu\n", lower, upper, even);



		hwm_result result = { 0 };

#		define FOUND(where_, what_, type_) do {              \
			hwm = (what_);                                     \
			_Pragma("omp atomic") ++hwm_index;                 \
			djsp_message(                                      \
				"(thread %2d) A(%lu) @ %lu = %lu (" type_ ")\n", \
				omp_get_thread_num(),                            \
				hwm_index, (where_), hwm                         \
			);                                                 \
			if (found_hwm)                                     \
				found_hwm(hwm_index, hwm, (where_));             \
		} while (0)

#		define RESULT(where_, what_) do {                      \
			if (watching == stride && (what_) > hwm) {           \
				FOUND(                                             \
					(where_), (what_),                               \
					"true! skipped potential"                        \
				);                                                 \
			} else if ((what_) > hwm && (what_) > result.what) { \
				result.what = (what_);                             \
				result.where = (where_);                           \
      	djsp_message(                                      \
      		"(thread %2d) A(%lu) @ %lu = %lu (potential)\n", \
      		omp_get_thread_num(),                            \
      		hwm_index, (where_), (what_)                     \
      	);                                                 \
			}                                                    \
		} while (0)



		u64 even_r = oneshot_2_memo(even, memo_read);
    if (memo_write) memo_write(even, even_r);

		if (even == lower)
			RESULT(even, even_r);

    for (u64 i = lower; i <= upper; i += 2) {
      r = oneshot_2_memo(i, memo_read);
      if (memo_write) memo_write(i, r);
     	RESULT(i, r);
    }

    if (even != lower)
    	RESULT(even, even_r);



		done[stride] = 1;
		if (watching == stride)
			++watching;
		else
			finished_strides[stride] = result;

		for (; watching <= strides && done[watching]; ++watching) {
			hwm_result chwm = finished_strides[watching];
			u64 what = chwm.what, where = chwm.where;

			if (what > hwm)
				FOUND(where, what, "true!");
	  }

#		undef FOUND
#		undef RESULT



// #		ifdef TIME
// 			wtime = omp_get_wtime() - wtime;
// 			fprintf(stderr, "finished stride #%lu, %fs\n", stride, wtime);
// #		endif
	  }

  free(finished_strides);
  free(done);
}
