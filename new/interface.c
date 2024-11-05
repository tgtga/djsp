#ifndef INTERFACE
#define INTERFACE

#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include "defs.c"

#define BANNER(s) "========== " s " =========="

FILE *log_file = NULL;
static inline void set_log_file(
  char *path
) {
  if ((log_file = fopen(path, "a")) == NULL)
    DIE("unable to open file `%s': %s\n", path, STRERRNO());

  setvbuf(log_file, NULL, _IOLBF, 0);
}

char *time_format = "%Y/%m/%d %H:%M:%S";
static inline void set_time_format(
  char *format
) {
  time_format = format;
}

void message(
  char *format,
  ...
) {
  time_t tm = time(NULL);
  struct tm *p = localtime(&tm);

  char time_string[65];
  strftime(time_string, sizeof(time_string), time_format, p);

  va_list args_out; va_start(args_out, format);

  if (log_file) {
    va_list args_log; va_copy(args_log, args_out);

    fprintf(log_file, "%s: ", time_string);
    vfprintf(log_file, format, args_log);

    va_end(args_log);
  }

  printf("%s: ", time_string);
  vprintf(format, args_out);

  va_end(args_out);
}

#endif
