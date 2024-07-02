#include <stdio.h>
#include <math.h>
#include <gmp.h>
#include <time.h>



unsigned int isqrt_babylonian(
  unsigned long n
) {
  unsigned long
    a = 1UL << (((64 - __builtin_clzl(n)) >> 1) + 1),
    b = (a + n / a) >> 1;

  while (b < a) {
    a = b;
    b = (a + n / a) >> 1;
  }

  return (unsigned int)a;
}

unsigned int isqrt_bs_standard(
  unsigned long n
) {
  int base = ((64 - __builtin_clzl(n)) - 1) >> 1;

  unsigned long
    l = 1UL <<  base,
    r = 1UL << (base + 1);

  do {
    unsigned long m = (l + r) >> 1;

    if (m * m <= n)
      l = m;
    else
      r = m;
  } while (l != r - 1);

  return (unsigned int)l;
}

unsigned int isqrt_bs_monobound(
  unsigned long n
) {
  int base = ((64 - __builtin_clzl(n)) - 1) >> 1;

  unsigned long
    l = 1UL <<  base,
    r = 1UL << (base + 1);

  do {
    unsigned long
      m = r >> 1,
      i = l + m;

    if (n >= i * i)
      l = i;

    r -= m;
  } while (r > 1);

  return (unsigned int)l;
}

#define isqrt isqrt_babylonian



void step_up_big(
  mpz_t n
) {
  mpz_pow_ui(n, n, 3UL);
  mpz_sqrt(n, n);
}



int main(void) {
  FILE *log = fopen("log.dat", "a");

  {
    time_t tm = time(NULL);
    struct tm *p = localtime(&tm);

    char buffer[4 + 1 + 2 + 1 + 2 + 1 + 2 + 1 + 2 + 1 + 2 + 1];
    strftime(buffer, sizeof(buffer), "%Y/%m/%d %H:%M:%S", p);

    fprintf(log, "==================== STARTED %s ====================\n", buffer);
  }

  // A(28) = 484 at 198424189 (10**8.297595)
  unsigned long  hwm_index = 198424189;
  unsigned short hwm_value = 484;
  unsigned char  hwm_count = 28;

  mpz_t v_big; mpz_init(v_big);

  for (unsigned long n = 1247674368; ; ++n) {
    unsigned short count = 0;

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

      time_t tm = time(NULL);
      struct tm *p = localtime(&tm);

      char buffer[4 + 1 + 2 + 1 + 2 + 1 + 2 + 1 + 2 + 1 + 2 + 1];
      strftime(buffer, sizeof(buffer), "%Y/%m/%d %H:%M:%S", p);

      printf(
        "%s: A(%u) = %u at %lu (10**%f)\n",
        buffer,
        hwm_count,
        hwm_value,
        hwm_index,
        log10((double)hwm_index)
      );

      fprintf(
        log,
        "%s: A(%u) = %u at %lu (10**%f)\n",
        buffer,
        hwm_count,
        hwm_value,
        hwm_index,
        log10((double)hwm_index)
      );
      fflush(log);
    }

    // if ((n & ((1 << 16) - 1)) == 0) {
      time_t tm = time(NULL);
      struct tm *p = localtime(&tm);

      char buffer[4 + 1 + 2 + 1 + 2 + 1 + 2 + 1 + 2 + 1 + 2 + 1];
      strftime(buffer, sizeof(buffer), "%Y/%m/%d %H:%M:%S", p);

      fprintf(stderr, "%s: on %lu\n", buffer, n);

      fprintf(log,    "%s: on %lu\n", buffer, n);
      fflush(log);
    // }
  }

  mpz_clear(v_big);

  {
    time_t tm = time(NULL);
    struct tm *p = localtime(&tm);

    char buffer[4 + 1 + 2 + 1 + 2 + 1 + 2 + 1 + 2 + 1 + 2 + 1];
    strftime(buffer, sizeof(buffer), "%Y/%m/%d %H:%M:%S", p);

    fprintf(log, "==================== TERMINATED %s ====================\n", buffer);
  }

  fclose(log);
}
