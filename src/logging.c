#include "../include/logging.h"

FILE *log_file = NULL;
char *time_format = "%Y-%m-%dT%H:%M:%S.%L%z";

/*
void set_up_log(int fd) {
  // duplicate file descriptor provided from ruby interop,
  // then open it as a file pointer
  log_file = fdopen(dup(fd), "a");

  if (log_file == NULL)
    DIE("unable to open logfile: %s\n", STRERRNO());

  // flush writes on every newline
  // SEGFAULTS!!!
  setvbuf(log_file, NULL, _IOLBF, 0);
}
*/
void set_up_log(const char *path) {
  if (path == NULL)
    log_file = NULL;

  log_file = fopen(path, "a");
  if (log_file == NULL)
    DIE("unable to open file '%s': %s\n", path, STRERRNO());

  setvbuf(log_file, NULL, _IOLBF, 0);
}

void message(char *format, ...) {
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
