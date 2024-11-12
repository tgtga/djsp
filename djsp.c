#include <stdio.h>
#include <getopt.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "include/defs.h"
#include "include/logging.h"
#include "include/seq.h"



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




static const char *usage =
  "usage: %s [short options]\n"
  "-l --log                    [f = filename]    set the output log file to 'f'\n"
  "-t --time-format            [t = time format] set the output time format to 't'\n"
  "-s --steps                  [n = integer]     show intermediate steps for values which exceed 2^'n'\n"
  "-L --start                  [n = integer]     start running from 'n'\n"
  "-R --end                    [n = integer]     stop running after 'n'\n"
  "-1 --oneshot (only with -L)                   only run for one integer\n"
  "-u --before-up                                run bignum reallocations before the up step\n"
  "-U --after-up                                 run bignum reallocations after the up step\n"
  "-d --before-down                              run bignum reallocations before the down step\n"
  "-D --after-down                               run bignum reallocations after the down step\n"
;

// investigate "suckless getopt":
// https://lobste.rs/s/pd7zaw/arg_h_suckless_alternative_getopt

int main(
  int argc,
  char **argv
) {
  opterr = 0;

  static struct option long_options[] = {
    { "log",         required_argument, NULL,                        'l'  },
    { "time-format", required_argument, NULL,                        't'  },
    { "steps",       optional_argument, NULL,                        's'  },
    { "start",       required_argument, NULL,                        'L'  },
    { "end",         required_argument, NULL,                        'R'  },
    { "oneshot",     no_argument,       NULL,                        '1'  },
    { "before-up",   no_argument,       (int *)&realloc_before_up,   true },
    { "after-up",    no_argument,       (int *)&realloc_after_up,    true },
    { "before-down", no_argument,       (int *)&realloc_before_down, true },
    { "after-down",  no_argument,       (int *)&realloc_after_down,  true },
    { 0, 0, 0, 0 }
  };

  for (int option_index, c; (c = getopt_long(
    argc, argv,
    "l:" "t:" "s::" "L:R:1" "dDuU",
    long_options, &option_index
  )) != -1; ) {
    switch (c) {
      case 0: {
        if (long_options[option_index].flag)
          break;

        fprintf(stderr, "hit 0!?\n");
        abort();
      } break;

      case 'l': log_file = open_file(optarg); break;

      case 't': time_format = optarg; break;

      case 's': {
        if (optarg)
          ssol = parse_int(optarg);
        else if (argv[optind] && argv[optind][0] != '-')
          ssol = parse_int(argv[optind++]);
        else
          ssol = 0;

        show_steps = 1;
      } break;

      case 'L': start_value = parse_int(optarg); break;
      case 'R': {
        end_value = parse_int(optarg);
        ending = 1;
      } break;
      case '1': {
        if (start_value == 1) {
          fprintf(stderr, "oneshot doesn't make sense without a starting value\n\n");
          fprintf(stderr, usage, *argv);
          exit(1);
        }

        end_value = start_value;
        ending = 1;
      } break;

      case 'd': realloc_before_down = true; break;
      case 'D': realloc_after_down  = true; break;
      case 'u': realloc_before_up   = true; break;
      case 'U': realloc_after_up    = true; break;

      case '?': {
        fprintf(stderr, "unrecognized option '%s'\n\n", argv[optind - 1]);
        fprintf(stderr, usage, *argv);
        exit(1);
      } break;

      default: DIE("oh nooo! default hit!\n");
    }
  }

  if (optind < argc)
    fprintf(stderr, "ignoring trailing arguments...\n");

  // now... sanity check!

  message("hello, world!\n");

  if (!show_steps) {
    printf("NOT showing steps!");
  } else {
    printf("showing steps");
    if (ssol)
      printf(" above 2^%lu", ssol);
  }
  printf("\n");

  printf("running on %lu", start_value);
  if (ending) {
    if (end_value != start_value)
      printf(" -> %lu", end_value);
  } else
    printf(" -> oo");
  printf("\n");

  printf(
    "%cO%c %cE%c\n",
    realloc_before_up ? 'Y' : 'N', realloc_after_up ? 'Y' : 'N',
    realloc_before_down ? 'Y' : 'N', realloc_after_down ? 'Y' : 'N'
  );
}
