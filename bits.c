#define BANNER(message) "========== " message " =========="

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

#define LOG_PATH "log.dat"

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
    a = (u64)1 << ((bit_length(n) + 1) >> 1),
    b = (a + n / a) >> 1;

  while (b < a) {
    a = b;
    b = (a + n / a) >> 1;
  }

  return (u32)a;
}
