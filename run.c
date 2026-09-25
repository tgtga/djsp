#include "include/sequence.h"

void print(u64 index, u64 mark, u64 where) {
	djsp_message("A(%lu) @ %lu = %lu\n", index, where, mark);
}

int main(void) {
	sequence_2_p(1, 100000000, NULL, NULL, print);

	return 0;
}
