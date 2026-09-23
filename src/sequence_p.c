#ifdef SEQP_INCLUDED

#ifndef SEQP_DEFS
#	define SEQP_DEFS
	typedef struct {
		u64 *hwms, *wheres;
		size_t len;
	} chunk_result;

#	define NUM_THREADS 12
#endif

#include <pthread.h>

memo_read_callback memo_read;
memo_write_callback memo_write;
chunk_result *chunk_results;
size_t chunk_results_len = 0;
volatile u64 ghwm;
u64 lowest_stride, strides; pthread_mutex_t ls_mutex;

void * worker(void *id) {
	while (1) {
		// fprintf(stderr, "thread %lu: trying to lock...\n", (u64)id);
		pthread_mutex_lock(&ls_mutex);
		// fprintf(stderr, "thread %lu: got lock\n", (u64)id);

		u64 stride = lowest_stride++;
		if (stride > strides) {
			fprintf(stderr, "thread %lu: GET OUT!!!\n", (u64)id);
			pthread_mutex_unlock(&ls_mutex);
			pthread_exit(NULL);
		}

		pthread_mutex_unlock(&ls_mutex);
		// fprintf(stderr, "thread %lu: unlocked\n", (u64)id);

		u64
			sqr = stride * stride, next = (stride + 1) * (stride + 1),
			even = sqr + (stride & 1),
			lower = sqr + !(stride & 1),
			upper = next - ((next & 1) + 1),
			hwm, r;

		fprintf(stderr, "thread %lu: %lu -> %lu, stride %lu\n", (u64)id, lower, upper, stride);

		u64 *hwms = malloc(0);
		u64 *wheres = malloc(0);
		size_t len = 0;

	#	define TRY_RESULT(where_, what_) do {                \
			if ((what_) > hwm && (what_) > ghwm) {                               \
				fprintf(stderr, "stride %lu found %lu @ %lu\n", stride, (where_), (what_)); \
			\
				hwm = (what_);                                   \
	                                                       \
				wheres = realloc(wheres, sizeof(*wheres) * (len + 1)); \
				hwms = realloc(hwms, sizeof(*hwms) * (len + 1)); \
				\
				hwms[len] = hwm;                               \
				wheres[len] = (where_); \
				++len; \
			}                                                   \
		} while (0)

		r = oneshot_2_memo(even, memo_read);
		if (memo_write) memo_write(even, r);
		TRY_RESULT(even, r);

		for (u64 i = lower; i <= upper; i += 2) {
			r = oneshot_2_memo(i, memo_read);
			if (memo_write) memo_write(i, r);
			TRY_RESULT(i, r);
		}

	#	undef TRY_RESULT

		chunk_results[stride] = (chunk_result){ hwms, wheres, len };
	}
}

void SEQP_NAME(
  u64 start, u64 end,
  memo_read_callback memo_read, memo_write_callback memo_write,
  hwm_callback found_hwm
) {
	fprintf(stderr, "new\n");
	fprintf(stderr, "sequence_2_p with %lu, %lu, %p, %p, %p\n", start, end, memo_read, memo_write, found_hwm);

  ghwm = 6; u64 hwm_index = 2;
  lowest_stride = 3; pthread_mutex_init(&ls_mutex, NULL);

  // strides is always a high estimate
  // worst case scenario more values are computed than what was asked for
  strides = isqrt(end);

	pthread_t threads[NUM_THREADS];
	chunk_results = malloc(sizeof(*chunk_results) * (strides + 1));

  if (found_hwm) found_hwm(1, 1, 2);
  if (found_hwm) found_hwm(2, 6, 3);

	for (int i = 0; i < NUM_THREADS; ++i)
		pthread_create(threads + i, NULL, worker, (void *)i);

	for (int i = 0; i < NUM_THREADS; ++i)
		pthread_join(threads[i], NULL);

	for (u64 stride = 3; stride <= strides; ++stride) {
		chunk_result result = chunk_results[stride];
		u64 *hwms = result.hwms, *wheres = result.wheres;
		size_t len = result.len;

		for (size_t i = 0; i < len; ++i) {
			u64 hwm = hwms[i], where = wheres[i];
			if (hwm > ghwm) {
				ghwm = hwm;
				++hwm_index;
				if (found_hwm) found_hwm(hwm_index, ghwm, where);
			}
		}

		free(hwms);
	}

	free(chunk_results);
	pthread_mutex_destroy(&ls_mutex);
}

#endif
