#include "include/sequence.h"
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

u64 *memo, *counts;

u64 memo_read_s(u64 i) {
	if (i <= MEMOSIZE) {
		++counts[i];
		return memo[i];
	} else return 0;

	// return (vi && vi <= MEMOSIZE) ? memo[vi] : 0;
}

void memo_write_s(u64 where, u64 what) {
	if (where <= MEMOSIZE)
		memo[where] = what;
}

u64 memo_read_o(u64 i) {
	if (i <= MEMOSIZE && (i & 1)) {
		++counts[i >> 1];
		return memo[i >> 1];
	} else return 0;

	// return (vi && vi <= MEMOSIZE && (vi & 1)) ? odd_memo[vi >> 1] : 0;
}

void memo_write_o(u64 where, u64 what) {
	if (where <= MEMOSIZE && (where & 1))
		memo[where >> 1] = what;
}

int compare(const void *a, const void *b) {
	int x = **(int **)a, y = **(int **)b;
	return (x > y) ? -1 : ((x < y) ? +1 : 0);
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

	free(pa);
	free(b);
	free(counts);
	free(memo);
# endif

	exit(0);
}

int main(void) {
	signal(SIGINT, end);

# if defined(S)
	memo = calloc(MEMOSIZE + 1, sizeof(*memo));
#	endif
#	if defined(O)
	memo = calloc(MEMOSIZE >> 1, sizeof(*memo));
#	endif
#	if defined(S) || defined(O)
	counts = calloc(MEMOSIZE + 1,  sizeof(*counts));
# endif

	sequence_rootopt_p(
		1, UPTO,
#		if defined(S)
			memo_read_s, memo_write_s,
#		elif defined(O)
			memo_read_o, memo_write_o,
#		endif
		NULL
	);

	end();
}
