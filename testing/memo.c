#include <stdio.h>
#include <math.h>
#include <gmp.h>
#include <time.h>



unsigned long isqrt(
  unsigned long n
) {
  unsigned long
    a = 1UL << (((64 - __builtin_clzl(n)) >> 1) + 1),
    b = (a + n / a) >> 1;

  while (b < a) {
    a = b;
    b = (a + n / a) >> 1;
  }

  return a;
}

int main(void) {
  unsigned long hwm_index = 0, hwm_value = 0, hwm_count = 0;

  

  mpz_t v_big; mpz_init(v_big);

  for (unsigned long n = 2; ; ++n) {
    unsigned long count = 0;

    unsigned long v_int = n;

    while (1) {
      process_int:

      if (v_int & 1) {
        if (v_int > 2642245UL) {
          mpz_ui_pow_ui(v_big, v_int, 3UL);
          mpz_sqrt(v_big, v_big);

          ++count;

          if ((mpz_odd_p(v_big) && (mpz_cmp_ui(v_big, 2642245UL) > 0)) || !mpz_fits_ulong_p(v_big)) {
            goto process_big;
          } else {
            v_int = mpz_get_ui(v_big);

            goto process_int;
          }
        }

        v_int = isqrt(v_int * v_int * v_int);
        ++count;

        goto process_int;
      } else {
        v_int = isqrt(v_int);
        ++count;

        if (v_int == 1)
          break;
        else
          goto process_int;
      }

      process_big:

      if (mpz_odd_p(v_big))
        mpz_pow_ui(v_big, v_big, 3UL);

      mpz_sqrt(v_big, v_big);

      ++count;

      if ((mpz_odd_p(v_big) && (mpz_cmp_ui(v_big, 2642245UL) > 0)) || !mpz_fits_ulong_p(v_big)) {
        goto process_big;
      } else {
        v_int = mpz_get_ui(v_big);

        goto process_int;
      }
    }

    if (count > hwm_value) {
      ++hwm_count;
      hwm_value = count;
      hwm_index = n;

      time_t tm;
      time(&tm);

      printf(
        "%s: A(%lu) = %lu at %lu (10**%f)\n",
        ctime(&tm),
        hwm_count,
        hwm_value,
        hwm_index,
        log10((double)hwm_index)
      );
    }
  }

  mpz_clear(v_big);
}
