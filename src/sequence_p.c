#ifdef SEQP_INCLUDED

#ifndef SEQP_DEFS
#	define SEQP_DEFS
	typedef struct {
		u64 *hwms, *wheres;
		size_t len;
	} chunk_result;

	typedef struct {
		int id;
		memo_read_callback memo_read; memo_write_callback memo_write;
		chunk_result **chunk_results_p;
		u64 * volatile hwm_p, *lowest_stride_p;
		u64 strides;
		pthread_mutex_t *ls_mutex_p; // lowest stride lock
	} worker_args;

#	define NUM_THREADS 16
#endif

#include <pthread.h>
#include <unistd.h>

void * worker(void *args_v) {
	worker_args args = *(worker_args *)args_v;

	int id = args.id;
	memo_read_callback memo_read = args.memo_read; memo_write_callback memo_write = args.memo_write;
	chunk_result **chunk_results_p = args.chunk_results_p;
	u64 * volatile hwm_p = args.hwm_p, * volatile lowest_stride_p = args.lowest_stride_p;
	u64 strides = args.strides;
	pthread_mutex_t *ls_mutex_p = args.ls_mutex_p;

	u64 hwm = *hwm_p;

	while (1) {
		pthread_mutex_lock(ls_mutex_p);

		u64 stride = (*lowest_stride_p)++;
		if (stride > strides) {
			fprintf(stderr, "thread %d: GET OUT!!!\n", id);
			pthread_mutex_unlock(ls_mutex_p);
			pthread_exit(NULL);
		}

		pthread_mutex_unlock(ls_mutex_p);

		u64
			sqr = stride * stride, next = (stride + 1) * (stride + 1),
			even = sqr + (stride & 1),
			lower = sqr + !(stride & 1),
			upper = next - ((next & 1) + 1),
			r;

		u64 *hwms = malloc(0);
		u64 *wheres = malloc(0);
		size_t len = 0;

// 			fprintf(stderr, "thread %d: checking %lu @ %lu against %lu, g %lu\n", id, (what_), (where_), hwm, *hwm_p);
	#	define TRY_RESULT(where_, what_) do {                \
			if ((what_) > hwm && (what_) > *hwm_p) {                               \
				fprintf(stderr, "thread %2d: stride %lu found %lu (against global %lu) @ %lu\n", id, stride, (what_), *hwm_p, (where_)); \
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

		if (len == 0) fprintf(stderr, "\e[F");
		fprintf(stderr, "thread %2d: setting chunk_results[%lu] (found %lu result", id, stride, len);
		if (len != 1) putc('s', stderr);
		fprintf(stderr, ")");
		if (len == 0) fprintf(stderr, "\e[K\n"); else fprintf(stderr, "\n");

		(*chunk_results_p)[stride] = (chunk_result){ hwms, wheres, len };
	}
}

void * dummy(void *p) { return p; }

void SEQP_NAME(
  u64 start, u64 end,
  memo_read_callback memo_read, memo_write_callback memo_write,
  hwm_callback found_hwm
) {
	chunk_result *chunk_results;
	volatile u64 lowest_stride;
	u64 strides;
	pthread_mutex_t ls_mutex, ms_mutex; pthread_cond_t ms_cond;

	pthread_mutex_init(&ls_mutex, NULL);
	pthread_mutex_init(&ms_mutex, NULL);
	pthread_cond_init(&ms_cond, NULL);

	fprintf(stderr, "new 1\n");
	fprintf(stderr, "sequence_2_p with %lu, %lu, %p, %p, %p\n", start, end, memo_read, memo_write, found_hwm);

  u64 hwm = 6; u64 hwm_index = 2; // ghwm = &hwm
  lowest_stride = 3; pthread_mutex_init(&ls_mutex, NULL);

  // strides is always a high estimate
  // worst case scenario more values are computed than what was asked for
  strides = isqrt(end);

	pthread_t threads[NUM_THREADS];
	chunk_results = calloc(strides + 1, sizeof(*chunk_results));

  if (found_hwm) found_hwm(1, 1, 2);
  if (found_hwm) found_hwm(2, 6, 3);

	printf("hwm = %lu, &hwm = %p\n", hwm, &hwm);

	for (int i = 0; i < NUM_THREADS; ++i) {
		worker_args args = {
			.id = i,
			.memo_read = memo_read, .memo_write = memo_write,
			.chunk_results_p = &chunk_results,
			.hwm_p = &hwm, .lowest_stride_p = &lowest_stride,
			.strides = strides,
			.ls_mutex_p = &ls_mutex
		};
		pthread_create(threads + i, NULL, worker, (void *)&args);
	}

	for (u64 stride = 3; stride <= strides; ++stride) {
		fprintf(stderr, "waiting for stride %lu\n", stride);
		// pthread_mutex_lock(&ms_mutex);

		for (
			u64 ** volatile hwms_check;
			*(hwms_check = &chunk_results[stride].hwms) == NULL;
//			fprintf(stderr, "[%lu]: hwms_check = %p\n", stride, *hwms_check)
		) {
			// fprintf(stderr, "chunk_results[%lu]. hwms = %p, wheres = %p, len = %lu vs. hwms_check = %p\n", stride, chunk_results[stride].hwms, chunk_results[stride].wheres, chunk_results[stride].len, hwms_check);
			// pthread_cond_wait(&ms_cond, &ms_mutex);
		}

		chunk_result result = chunk_results[stride];
		u64 *hwms = result.hwms, *wheres = result.wheres;
		size_t len = result.len;

		for (size_t i = 0; i < len; ++i) {
			u64 c = hwms[i], where = wheres[i];
			if (c > hwm) {
				hwm = c;
				++hwm_index;
				if (found_hwm) found_hwm(hwm_index, hwm, where);
			}
		}

		free(hwms);
	}

	for (int i = 0; i < NUM_THREADS; ++i)
		pthread_join(threads[i], NULL);

	free(chunk_results);
	pthread_mutex_destroy(&ls_mutex);
}

#endif
