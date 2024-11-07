#ifndef DEFS
#define DEFS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>

#define DIE(...) do {                    \
  fprintf(stderr, __VA_ARGS__);          \
  fprintf(stderr, "i'm gonna die...\n"); \
  exit(1);                               \
} while (0)

#define STRERRNO() (strerror(errno))

/*
void die(
  char *format,
  ...
) {
  va_list args; va_start(args, format);

  vfprintf(stderr, format, args);

  va_end(args);
}
*/

#endif
