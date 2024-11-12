#include <stdio.h>
#include <getopt.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "defs.h"
#include "interface.c"
#include "seq.c"



unsigned long parse_int(
  char *str
) {
  errno = 0;
  char *end;
  unsigned long ret = strtoul(str, &end, 0);

  if (errno)
    DIE("unable to convert '%s' to an integer: %s\n", str, strerror(errno));

  if (*end != '\0') {
    if (end == str)
      DIE("unable to find an integer in '%s'\n", str);
    else
      fprintf(stderr, "trailing junk '%s' in '%s'\n", end, str);
  }

  if (end == str)
    DIE("empty integer given\n");

  return ret;
}

FILE *open_file(
  char *path
) {
  FILE *out = fopen(path, "a");
  if (out == NULL)
    DIE("unable to open file '%s': %s\n", path, STRERRNO());

  setvbuf(out, NULL, _IOLBF, 0);

  return out;
}



char *program_name = NULL;

static inline void usage(void) {
  fprintf(stderr,
    "usage: %s [short options]\n"
    "-l --log         [f = filename]    set the output log file to 'f'\n"
    "-n --start       [n = integer]     start running from 'n'\n"
    "-t --time_format [t = time format] set the output time format to 't'\n"
    "-s --steps       [n = integer]     show intermediate steps for values which exceed 2^'n'\n"
    "-o --oneshot                       only run for one integer\n",
  program_name);
}



# define BAD_ARG(...) do {      \
  fprintf(stderr, __VA_ARGS__); \
  fprintf(stderr, "\n");        \
  usage();                      \
  exit(1);                      \
} while (0)

void set_log(void) {
  log_file = open_file(optarg);

  message("hello, world! :3\n");
}

void set_start(void) {
  start = parse_int(optarg);

  printf("your number is %lu\n", start);
}

void set_time_format(void) {
  time_format = optarg;
}

void set_steps(void) {
  ssol = parse_int(optarg);
  show_steps = 1;

  printf("showing steps over 2**%lu now!\n", ssol);
}

void set_oneshot(void) {
  oneshot = 1;
}

void question(void) {
  BAD_ARG("");
}

int main(
  int argc,
  char **argv
) {
  program_name = *argv;

  opterr = 0;

  static struct option long_options[] = {
    { "log",         required_argument, NULL,     'l' },
    { "start",       required_argument, NULL,     'n' },
    { "time_format", required_argument, NULL,     't' },
    { "steps",       required_argument, NULL,     's' },
    { "oneshot",     no_argument,       &oneshot, 1   },
  };

  while (1) {
    int option_index;
    char c = getopt_long(argc, argv, "l:n:t:s:o", long_options, &option_index);

    if (c == -1)
      break;

    if (c == 0) {
      if (long_options[option_index].flag)
        break;

      fprintf(stderr, "hit 0!?\n");
      abort();
    }

    void (*s[256])(void) = { 0,
      ['l'] = set_log,   ['n'] = set_start, ['t'] = set_time_format,
      ['s'] = set_steps, ['o'] = set_oneshot
    };

    void (*f)(void) = s[c];
    if (!f) {
      fprintf(stderr, "default hit!\n");
      abort();
    }
    f();
  }

  if (oneshot)
    printf("niko oneshot...\n");

  for (int i = optind; i < argc; ++i) {
    char *arg = argv[i];
    BAD_ARG("trailing argument `%s'\n", arg);
  }
}
