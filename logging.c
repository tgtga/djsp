#include "logging.h"

FILE *log_file = NULL;
char *time_format = "%Y/%m/%d %H:%M:%S";

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
