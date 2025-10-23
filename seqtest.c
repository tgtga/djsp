#include "include/oneshot.h"
#include <signal.h>

#ifndef UPTO
#	define UPTO 100000000
#endif
#ifndef MEMOSIZE
#	define MEMOSIZE 1000000
#endif
#ifndef HIGHEST
#	define HIGHEST 25
#endif

u64 *memo, *odd_memo, *counts;

u64 memo_lookup_s(
	u64 vi,
	mp_limb_t *vp, mp_size_t vl
) {
	if (vi && vi <= MEMOSIZE) {
		++counts[vi];
		return memo[vi];
	} else return 0;

	// return (vi && vi <= MEMOSIZE) ? memo[vi] : 0;
}

u64 memo_lookup_o(
	u64 vi,
	mp_limb_t *vp, mp_size_t vl
) {
	if (vi && vi <= MEMOSIZE && (vi & 1)) {
		++counts[vi >> 1];
		return odd_memo[vi >> 1];
	} else return 0;

	// return (vi && vi <= MEMOSIZE && (vi & 1)) ? odd_memo[vi >> 1] : 0;
}

int compare(const void *a, const void *b) {
	int x = **(int **)a, y = **(int **)b;

	if (x > y) return -1;
	if (x < y) return +1;
	           return  0;
}

_Noreturn void end() {
# if defined(S) || defined(O)
	u64 *a = counts, *b = malloc(sizeof(*a) * (MEMOSIZE + 1));
	for (int i = 0; i <= MEMOSIZE; ++i)
#		if defined(S)
			b[i] = i;
#		elif defined(O)
			b[i] = 2 * i + 1;
#		endif

  u64 **pa = malloc(sizeof(*pa) * (MEMOSIZE + 1));
  for (int i = 0; i <= MEMOSIZE; ++i)
  	pa[i] = &a[i];

	qsort(pa, MEMOSIZE + 1, sizeof(*pa), compare);

	for (int ta, tb, i = 0, j, k; i <= MEMOSIZE; ++i) {
  	if (i != pa[i] - a) {
			ta = a[i];
      tb = b[i];
      k = i;
      while (i != (j = pa[k] - a)) {
      	a[k] = a[j];
        b[k] = b[j];
        pa[k] = &a[k];
        k = j;
      }
      a[k] = ta;
      b[k] = tb;
      pa[k] = &a[k];
    }
  }

	printf("%d highest occurences were:\n", HIGHEST);
  for (int i = 0; i < HIGHEST; ++i)
  	printf("#%lu: %lux @ %lu (len. %lu)\n", i + 1, a[i], b[i], oneshot_2(b[i]));

	free(b);
	free(counts);
# endif

#	if defined(S)
	free(memo);
# endif
#	if defined(O)
	free(odd_memo);
# endif

	exit(0);
}

int main(void) {
	signal(SIGINT, end);

# if defined(S)
	    memo = calloc(MEMOSIZE + 1,  sizeof(*memo));
#	endif
#	if defined(O)
	odd_memo = calloc(MEMOSIZE >> 1, sizeof(*odd_memo));
#	endif
#	if defined(S) || defined(O)
	counts   = calloc(MEMOSIZE + 1,  sizeof(*counts));
# endif

  u64 hwm = 6, hwm_index = 2;

  printf("A(1) @ 2 = 1\n");
  printf("A(2) @ 3 = 6\n");

#	if defined(S)
#		pragma message "standard"
#   define DO(term) do { \
			r = oneshot_2_memo((term), memo_lookup_s); \
			if ((term) <= MEMOSIZE) \
				memo[(term)] = r; \
		} while (0)
#	elif defined(O)
#		pragma message "odds only"
#		define DO(term) do { \
			r = oneshot_2_memo((term), memo_lookup_o); \
			if ((term) <= MEMOSIZE && ((term) & 1)) \
				odd_memo[(term) >> 1] = r; \
		} while (0)
#	else
#		pragma message "no memo"
#		define DO(term) do { \
			r = oneshot_2((term)); \
		} while (0)
#	endif

  u64 n = 4;
  for (u64 stride = 2; ; ++stride) {
    u64 lower, upper, even, r;
    lower = n + !(n & 1); even = n + (n & 1);
    n += 2 * stride + 1;
    upper = n - ((n & 1) + 1);

		DO(even);
    if (r > hwm) {
      hwm = r; ++hwm_index;
      printf("A(%lu) @ %lu = %lu\n", hwm_index, even, hwm);
    }

    for (u64 i = lower; i <= upper; i += 2) {
      if (i > UPTO) end();

			DO(i);
      if (r > hwm) {
        hwm = r; ++hwm_index;
        printf("A(%lu) @ %lu = %lu\n", hwm_index, i, hwm);
      }
    }
  }
}
