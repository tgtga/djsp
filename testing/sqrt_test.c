#include <stdio.h>
#include <math.h>
#include <immintrin.h>

static inline float sse_sqrt(
  float x
) {
  float out;
  _mm_store_ss(&out, _mm_ceil_ss(_mm_setzero_ps(), _mm_sqrt_ss(_mm_load_ss(&x))));
   return out;
}

int main(void) {
  union {
    float f;
    unsigned int i;
  } testing;

  for (unsigned int i = 0; i <= ~(unsigned int)0; ++i) {
    testing.i = i;

    if (testing.f < 1 || isnan(testing.f))
      continue;

    float approx = sse_sqrt(testing.f), real = floorf(sqrtf(testing.f));

    if (approx < real)
      printf("%f (%f = %u) vs. %f\n", approx, testing.f, i, real);
  }
}
