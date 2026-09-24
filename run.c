#include "include/sequence.h"

void print(u64 a, u64 b, u64 c) {
	printf("%lu, %lu, %lu\n", a, b, c);
}

int main(void) {
	sequence_2_p(1, 100000, NULL, NULL, print);

	return 0;
}
