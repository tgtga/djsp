#include "include/seq.h"

typedef struct {
  u64 n, base, expected;
} tester;

int main(void) {
  tester tests[] = {
    {     1,  2,   0 },
    {    25,  2,  11 },
    { 78901,  2, 258 },
    {    13,  3,   5 },
    {    29,  3,   5 },
    {    32,  3,   5 },
    {    44,  3,   7 },
    {    19,  4,   4 },
    {    23,  4,   4 },
    {    44,  9,   5 },
    {    89, 10,   5 },
    {    32, 11,   6 }
  };

  show_steps = 1; ssol = 0;

  size_t count = sizeof(tests) / sizeof(*tests), passed = 0;
  for (int p = 0; p < sizeof(tests) / sizeof(*tests); ++p) {
    tester current = tests[p];
    u64 n = current.n, base = current.base, expected = current.expected;

    u64 result = sequence_nary(n, base);

    printf("juggler_%lu(%lu) = %lu: ", base, n, result);
    if (result == expected) {
      printf("PASSED");
      ++passed;
    } else {
      printf("FAILED");
    }
    puts("");
  }

  printf("passed: %zu/%zu\n", passed, count);
}
