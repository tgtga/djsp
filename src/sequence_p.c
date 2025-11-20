#ifdef SEQP_INCLUDED

#ifndef SEQP_DEFS
#	define SEQP_DEFS
	typedef struct {
		u64 what, where;
	} hwm_result;
#endif

void SEQP_NAME(
  u64 start, u64 end,
  memo_read_callback memo_read, memo_write_callback memo_write,
  hwm_callback found_hwm
) {

	fprintf(stderr, "sequence_2_p with %lu, %lu, %p, %p, %p\n", start, end, memo_read, memo_write, found_hwm);

  u64 hwm = 6, hwm_index = 2;

  // strides is always a high estimate
  // worst case scenario more values are computed than what was asked for
  const u64 strides = isqrt(end); // u64 n = 4;

	hwm_result *finished_strides = malloc(sizeof(*finished_strides) * (strides + 1));
	unsigned char *done = calloc(strides + 1, sizeof(*done)); done[0] = done[1] = done[2] = 1;
	size_t watching = 3;

  djsp_message("A(1) @ 2 = 1\n"); if (found_hwm) found_hwm(1, 1, 2);
  djsp_message("A(2) @ 3 = 6\n"); if (found_hwm) found_hwm(2, 6, 3);

# pragma omp parallel for \
		num_threads(3) \
		default(none) \
		shared(stderr, hwm, hwm_index, watching, done, finished_strides) \
		firstprivate(strides, end, found_hwm, memo_read, memo_write)
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

#		define DEBUG(fmt_, ...) fprintf(stderr, "(thread %2d) " fmt_, omp_get_thread_num(), ##__VA_ARGS__)

#		define FOUND(where_, what_, type_) do {              \
			DEBUG("entering hwm set\n"); \
			_Pragma("omp critical") { \
				hwm = (what_);                                     \
				++hwm_index;                 \
			} \
			DEBUG("finished hwm set, A(%lu) @ %lu = %lu (" type_ ")\n", hwm_index, (where_), hwm);                                                 \
			if (found_hwm) {                                  \
				DEBUG("entering found_hwm from c (%lu, %lu, %lu)\n", hwm_index, hwm, (where_)); \
				found_hwm(hwm_index, hwm, (where_));             \
				DEBUG("exiting found_hwm from c (%lu, %lu, %lu)\n", hwm_index, hwm, (where_)); \
			} \
		} while (0)

#		define RESULT(where_, what_) do {                      \
			if (watching == stride && (what_) > hwm) {           \
				DEBUG("entering FOUND call in RESULT\n"); \
				FOUND(                                             \
					(where_), (what_),                               \
					"true! skipped potential"                        \
				);                                                 \
				DEBUG("exiting FOUND call in RESULT\n"); \
			} else if ((what_) > hwm && (what_) > result.what) { \
				DEBUG("entering deferment block in RESULT\n"); \
				result.what = (what_);                             \
				result.where = (where_);                           \
      	DEBUG("exiting deferment block in RESULT, A(%lu) @ %lu = %lu (potential)\n", hwm_index, (where_), (what_)); \
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

		DEBUG("big block finished\n");
		_Pragma("omp critical") {
			done[stride] = 1;
			if (watching == stride)
				++watching;
			else
				finished_strides[stride] = result;
		}

		while (watching <= strides && done[watching]) {
			hwm_result chwm = finished_strides[watching];
			u64 what = chwm.what, where = chwm.where;

			if (what > hwm)
				FOUND(where, what, "true!");

			++watching;
		}

		DEBUG("(%lu -> %lu) finished\n", lower, upper);
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

#endif
