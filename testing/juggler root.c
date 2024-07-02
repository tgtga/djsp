#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <stdarg.h>
#include <signal.h>
#include <gmp.h>



typedef   uint8_t     u8;
typedef   uint16_t   u16;
typedef   uint32_t   u32;
typedef   uint64_t   u64;
typedef __uint128_t u128;



void print_time(
  const char *separator,
  size_t count,
  ...
) {
  time_t tm = time(NULL);
  struct tm *p = localtime(&tm);

  char buffer[4 + 1 + 2 + 1 + 2 + 1 + 2 + 1 + 2 + 1 + 2 + 1];
  strftime(buffer, sizeof(buffer), "%Y/%m/%d %H:%M:%S", p);

  va_list files;
  va_start(files, count);

  for (size_t pos = 0; pos < count; ++pos) {
    FILE *current_file = va_arg(files, FILE *);
    fprintf(current_file, "%s%s", buffer, separator);
  }

  va_end(files);
}

FILE *global_log;
void print_to_log(
  FILE *where,
  const char *format,
  ...
) {
# ifdef DONT_LOG
    va_list args_out;

    va_start(args_out, format);

    print_time(": ", 1, where);

    vfprintf(where, format, args_out);

    va_end(args_out);
# else
    va_list args_out, args_log;

    va_start(args_out, format);
    va_copy(args_log, args_out);

    print_time(": ", 2, where, global_log);

    vfprintf(where, format, args_out);
    vfprintf(global_log, format, args_log);

    fflush(global_log);

    va_end(args_out);
    va_end(args_log);
# endif
}



int bit_length(
  u64 n
) {
  return 64 - __builtin_clzl(n);
}

u32 isqrt(
  u64 n
) {
  u64
    a = (u64)1 << ((bit_length(n) >> 1) + 1),
    b = (a + n / a) >> 1;

  while (b < a) {
    a = b;
    b = (a + n / a) >> 1;
  }

  return (u32)a;
}



u16 sequence(
  u64 seed
) {
  static mpz_t v_big;
  static int first_time = 1;

  if (first_time) {
    mpz_init(v_big);
    first_time = 0;
  }

  u64 v_int = seed;
  u16 count = 0;

  while (1) {
    process_int:

#   ifdef PRINT_STEPS
    print_to_log(stderr, "%lu step %u: less than 2**%u\n", seed, count, bit_length(v_int));
#   endif

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

      if (v_int == 1)
        break;

      v_int = isqrt(v_int * v_int * v_int);
      ++count;

      goto process_int;
    } else {
      v_int = isqrt(v_int);
      ++count;

      goto process_int;
    }

    process_big:

#   ifdef PRINT_STEPS
    print_to_log(stderr, "%lu step %u: less than 2**%zu\n", seed, count, mpz_sizeinbase(v_big, 2));
#   endif

    if (mpz_odd_p(v_big)) {
      mp_size_t limbs = mpz_size(v_big);



      // square = v ** 2

      mpz_t square; mpz_init(square);
      mp_limb_t *square_mpp = mpz_limbs_write(square, limbs * 2);

      mpn_sqr(square_mpp, mpz_limbs_read(v_big), limbs);

      mpz_limbs_finish(square, limbs * 2);



      // cube = square * v = v ** 3

      mpz_t cube; mpz_init(cube);
      mp_limb_t *cube_mpp = mpz_limbs_write(cube, limbs * 3);

      mpn_mul(cube_mpp, mpz_limbs_read(square), limbs * 2, mpz_limbs_read(v_big), limbs);

      mpz_clear(square);

      mpz_limbs_finish(cube, limbs * 3);



      // v = isqrt(cube) = isqrt(v ** 3) = floor(v ** 3/2)

      const mp_limb_t *cube_mpp_r = mpz_limbs_read(cube);
      mp_size_t cube_limbs = limbs * 3;

      while (cube_mpp_r[cube_limbs - 1] == 0)
        --cube_limbs;

      mp_limb_t *v_mpp = mpz_limbs_write(v_big, (cube_limbs + 1) / 2);

      mpn_sqrtrem(v_mpp, NULL, cube_mpp_r, cube_limbs);

      mpz_clear(cube);

      mpz_limbs_finish(v_big, (cube_limbs + 1) / 2);

      mpz_realloc2(v_big, mpz_sizeinbase(v_big, 2));
    } else {
      mp_size_t limbs = mpz_size(v_big);



      mpz_t root; mpz_init(root);
      mp_limb_t *root_mpp = mpz_limbs_write(root, (limbs + 1) / 2);

      mpn_sqrtrem(root_mpp, NULL, mpz_limbs_read(v_big), limbs);

      mpz_limbs_finish(root, (limbs + 1) / 2);



      mpz_clear(v_big);

      *v_big = *root;
    }

    ++count;

    if ((mpz_odd_p(v_big) && (mpz_cmp_ui(v_big, 2642245UL) > 0)) || !mpz_fits_ulong_p(v_big)) {
      goto process_big;
    } else {
      v_int = mpz_get_ui(v_big);

      goto process_int;
    }
  }

  return count;
}



u64 global_last_n = 0;
void terminate(
  int dummy
) {
  print_to_log(stderr, "==================== TERMINATED on %lu ====================\n", global_last_n);

  fflush(stdout);
  fflush(stderr);

# ifndef DONT_LOG
  fclose(global_log);
# endif

  exit(0);
}

int main(void) {
# ifndef DONT_LOG
  global_log = fopen("log.dat", "a");
# endif

  signal(SIGINT, terminate);

  print_to_log(stderr, "==================== STARTED ====================\n");

  /*
  // A(28) = 484 at 198424189 (10**8.297595)
  u64 hwm_index = 198424189;
  u16 hwm_value = 484;
  u8  hwm_count = 28;

  for (u64 n = 1247677915; n < 1247677916; ++n) {
  */

  u64 hwm_index = 0;
  u16 hwm_value = 0;
  u8  hwm_count = 0;

  /*
  for (u64 n = 2; ; ++n) {
    u16 count = sequence(n);

    if (count > hwm_value) {
      ++hwm_count;
      hwm_value = count;
      hwm_index = n;

      print_to_log(
        stdout,
        "A(%u) = %u at %lu (10**%f)\n",
        hwm_count,
        hwm_value,
        hwm_index,
        log10((double)hwm_index)
      );
    }

    if ((n & (((u64)1 << 16) - 1)) == 0)
      print_to_log(stderr, "on %lu\n", n);

    global_last_n = n;
  }
  */
  
  for (u32 root = 1; ; ++root) {
    for (
      u64 n = (u64)root * root, cap = ((u64)root + 1) * ((u64)root + 1);
      n < cap;
      ++n
    ) {
      u16 count = sequence(n);

      if (count > hwm_value) {
        ++hwm_count;
        hwm_value = count;
        hwm_index = n;

        print_to_log(
          stdout,
          "A(%u) = %u at %lu (10**%f)\n",
          hwm_count,
          hwm_value,
          hwm_index,
          log10((double)hwm_index)
        );
      }

      if ((n & (((u64)1 << 16) - 1)) == 0)
        print_to_log(stderr, "on %lu\n", n);

      global_last_n = n;
    }
  }

  terminate(0);
}
