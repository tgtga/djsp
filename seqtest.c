#include "include/oneshot.h"

#define UPTO 10000000
#define MEMOSIZE 1000000

u64 *memo, *odd_memo;

u64 memo_lookup_s(
	u64 vi,
	mp_limb_t *vp, mp_size_t vl
) {
	return (vi && vi <= MEMOSIZE)
		? memo[vi]
		: 0;
}

u64 memo_lookup_o(
	u64 vi,
	mp_limb_t *vp, mp_size_t vl
) {
	return (vi && vi <= MEMOSIZE && (vi & 1)) ? odd_memo[vi >> 1] : 0;
}

int main(void) {
	memo = calloc(MEMOSIZE + 1, sizeof(*memo));
	odd_memo = calloc(MEMOSIZE >> 1, sizeof(*odd_memo));

  u64 hwm = 0, hwm_index = 0;
  for (u64 i = 2; i <= UPTO; ++i) {
#   if defined(S)
#     pragma message "standard"
    	u64 r = oneshot_2_memo(i, memo_lookup_s);
    	if (i <= MEMOSIZE)
    		memo[i] = r;
#   elif defined(O)
#     pragma message "odds only"
    	u64 r = oneshot_2_memo(i, memo_lookup_o);
    	if (i <= MEMOSIZE && (i & 1))
    		odd_memo[i >> 1] = r;
#   else
#     pragma message "no memo"
  		u64 r = oneshot_2(i);
#   endif

    if (r > hwm) {
      hwm = r; ++hwm_index;
      djsp_message("A(%lu) @ %lu = %lu\n", hwm_index, i, hwm);
    }
  }

	free(memo);
}
