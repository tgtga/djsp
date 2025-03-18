#ifndef DEFS
#define DEFS

#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#define DJSP_VERSION_MAJOR 1
#define DJSP_VERSION_MINOR 0
#define DJSP_VERSION_PATCH 0
#define DJSP_VERSION_INT (DJSP_VERSION_MAJOR * 10000 + DJSP_VERSION_MINOR * 100 + DJSP_VERSION_PATCH)
#define DJSP_VERSION_STR STR(DJSP_VERSION_MAJOR) "." STR(DJSP_VERSION_MINOR) "." STR(DJSP_VERSION_PATCH)

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
