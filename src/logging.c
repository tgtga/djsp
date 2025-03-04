#include "../include/logging.h"

FILE *log_file = NULL;

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
  // https://stackoverflow.com/a/77826253

  struct timespec tp;
  clock_gettime(CLOCK_REALTIME, &tp);
  struct tm *local = localtime(&tp.tv_sec);

  char ts[sizeof("9999-12-31T23:59:59.999+00:00")]; // 29 chars + '\0'
  size_t size = sizeof(ts);
  int off = 0;

  off = strftime(ts, size, "%FT%T", local); // same as "%Y-%m-%dT%H:%M:%S" 
  off += snprintf(ts + off, size - off, ".%03ld", tp.tv_nsec / 1000000);
  off += strftime(ts + off, size - off, "%z", local);
  
  va_list args_out; va_start(args_out, format);

  if (log_file) {
    va_list args_log; va_copy(args_log, args_out);

    fprintf(log_file, "%s: ", ts);
    vfprintf(log_file, format, args_log);

    va_end(args_log);
  }

  printf("%s: ", ts);
  vprintf(format, args_out);

  va_end(args_out);
}
