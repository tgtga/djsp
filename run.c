#include "include/sequence.h"

void print(u64 index, u64 mark, u64 where) {
	djsp_message("A(%lu) @ %lu = %lu\n", index, where, mark);
}

int main(void) {
	sequence_2_p(1, 10000000, NULL, NULL, print);

	/*
	for (u64 i = 1; ; ++i)
		if (oneshot_2(i) == 5)
			printf("%lu\n", i);
	*/

	return 0;
}
