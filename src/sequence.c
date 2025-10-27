#include "../include/sequence.h"

void sequence(
  u64 base,
  u64 start, u64 end, u64 step, bool endless,
  // memo_callback memo,
  hwm_callback found_hwm
) {
  u64 hwm = 0, hwm_index = 0;

  for (u64 i = start; endless || i < end; i += step) {
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

void sequence_alert(
  u64 base,
  u64 start, u64 end, u64 step, bool endless,
  u64 alert,
  // memo_callback memo,
  hwm_callback found_hwm
) {
  u64 hwm = 0, hwm_index = 0;

  for (u64 i = start, bound; endless || i < end; ) {
    for (bound = i + alert; i < bound; i += step) {
      u64 r = base ? oneshot_n(i, base) : oneshot_2(i);
      if (r > hwm) {
        hwm = r; ++hwm_index;
        if (found_hwm) found_hwm(hwm_index, hwm, i);
      }
    }
    djsp_message("finished %lu\n", bound);
  }
}

void sequence_rootopt(
  u64 start, u64 end, u64 step, bool endless,
  // memo_callback memo,
  hwm_callback found_hwm
) {
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

typedef struct {
	hwm_result *hwms;
	size_t hwm_count;
} stride_result;

void sequence_rootopt_p(
  u64 start, u64 end,
  memo_read_callback memo_read, memo_write_callback memo_write,
  hwm_callback found_hwm
) {
  u64 hwm = 6, hwm_index = 2;

  // strides is always a high estimate
  // worst case scenario more values are computed than what was asked for
  u64 strides = isqrt(end); // u64 n = 4;

	stride_result *finished_strides = malloc(sizeof(*finished_strides) * (strides + 1));
	unsigned char *done = calloc(strides + 1, sizeof(*done)); done[0] = done[1] = done[2] = 1;
	size_t watching = 3;

  djsp_message("A(1) @ 2 = 1\n"); if (found_hwm) found_hwm(1, 1, 2);
  djsp_message("A(2) @ 3 = 6\n"); if (found_hwm) found_hwm(2, 6, 3);

# pragma omp parallel for
  for (u64 stride = 3; stride <= strides; ++stride) {
#		ifdef TIME
	  	double wtime = omp_get_wtime();
#		endif
		u64
			sqr = stride * stride, next = (stride + 1) * (stride + 1),
			even = sqr + (stride & 1),
			lower = sqr + !(stride & 1),
			upper = next - ((next & 1) + 1),
			r;
    // fprintf(stderr, "stride #%lu: %lu -> %lu, condensing evens into %lu\n", stride, lower, upper, even);

		if (lower > end)
	    fprintf(stderr, "OVERSHOT! %lu -> %lu, condensing evens into %lu\n", lower, upper, even);



		for (; done[watching]; ++watching) {
			stride_result sr = finished_strides[watching];
			// fprintf(stderr, "indexing stride #%lu (.hwms = %p, .hwm_count = %lu)\n", watching, (void *)sr.hwms, sr.hwm_count);

			for (size_t k = 0; k < sr.hwm_count; ++k) {
				hwm_result chwm = sr.hwms[k];
				u64 what = chwm.what, where = chwm.where;
				// fprintf(stderr, "what = %lu, where = %lu\n", what, where);

				if (what > hwm) {
					hwm = what;
					++hwm_index;

	      	djsp_message("A(%lu) @ %lu = %lu (true!)\n", hwm_index, where, hwm);
	      	if (found_hwm) found_hwm(hwm_index, hwm, where);
	      }
			}
	  }



		hwm_result *results = NULL;
		size_t result_count = 0, result_alloc = 0;

		/*
#   define PUSH_RESULT(what, where) do {                               \
			if (stride == watching) {                                        \
				hwm = what;                                                    \
				++hwm_index;                                                   \
				djsp_message(                                                  \
					"A(%lu) @ %lu = %lu (true! indexing skipped)\n",             \
					hwm_index, where, hwm                                        \
				);                                                             \
			} else {                                                         \
				if (results == NULL) {                                         \
					results = malloc(sizeof(*results) * 1);                      \
					result_alloc = 1;                                            \
				}                                                              \
				if (result_count >= result_alloc) {                            \
					result_alloc = (result_alloc + 1) * 3 / 2;                   \
					results = realloc(results, sizeof(*results) * result_alloc); \
				}                                                              \
				results[result_count++] = (hwm_result){ what, where };         \
			}                                                                \
		} while (0)
		*/

#   define PUSH_RESULT(what, where) do {                             \
			if (results == NULL) {                                         \
				results = malloc(sizeof(*results) * 1);                      \
				result_alloc = 1;                                            \
			}                                                              \
			if (result_count >= result_alloc) {                            \
				result_alloc = (result_alloc + 1) * 3 / 2;                   \
				results = realloc(results, sizeof(*results) * result_alloc); \
			}                                                              \
			results[result_count++] = (hwm_result){ what, where };         \
		} while (0)



		hwm_result even_hwmr = { 0 };
    r = oneshot_2_memo(even, memo_read);
    if (memo_write) memo_write(even, r);
    if (r > hwm) {
    	even_hwmr = (hwm_result){ r, even };
    	djsp_message("A(%lu) @ %lu = %lu (potential)\n", hwm_index, even, r);
    }

		if (even == lower)
			PUSH_RESULT(even_hwmr.what, even_hwmr.where);

    for (u64 i = lower; i <= upper; i += 2) {
      r = oneshot_2_memo(i, memo_read);
      if (memo_write) memo_write(i, r);
      if (r > hwm) {
      	PUSH_RESULT(r, i);
      	djsp_message("A(%lu) @ %lu = %lu (potential)\n", hwm_index, i, r);
      }
    }

    if (even != lower)
    	PUSH_RESULT(even_hwmr.what, even_hwmr.where);

		if (results)
			finished_strides[stride] = (stride_result){ results, result_count };
		done[stride] = 1;



#		ifdef TIME
			wtime = omp_get_wtime() - wtime;
			fprintf(stderr, "finished stride #%lu, %fs\n", stride, wtime);
#		endif
  }

  free(finished_strides);
}
