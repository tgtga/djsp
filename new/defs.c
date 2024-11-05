#ifndef DEFS
#define DEFS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define DIE(...) do {                    \
  fprintf(stderr, __VA_ARGS__);          \
  fprintf(stderr, "i'm gonna die...\n"); \
  exit(1);                               \
} while (0)

#define STRERRNO() (strerror(errno))

unsigned long parse_int(
  char *str,
  char *code_location
) {
  errno = 0;
  char *end;
  unsigned long ret = strtoul(str, &end, 0);

  if (errno)
    DIE("%s: unable to convert `%s' to an integer: %s\n", code_location, str, strerror(errno));

  if (*end != '\0') {
    if (end == str)
      DIE("%s: unable to find an integer in `%s'\n", code_location, str);
    else
      fprintf(stderr, "%s: trailing junk `%s' in `%s'\n", code_location, end, str);
  }

  if (end == str)
    DIE("%s: empty integer given\n", code_location);

  return ret;
}

#endif
