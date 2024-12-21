#define _GNU_SOURCE 1

#include <stdio.h>
#include <getopt.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "../include/defs.h"
#include "../include/logging.h"
#include "../include/seq.h"



FILE *open_file(
  char *path
) {
  FILE *out = fopen(path, "a");
  if (out == NULL)
    DIE("unable to open file '%s': %s\n", path, STRERRNO());

  setvbuf(out, NULL, _IOLBF, 0);

  return out;
}

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
      fprintf(stderr, "trailing junk '%s' in integer '%s'\n", end, str);
  }

  if (end == str)
    DIE("empty integer given\n");

  return ret;
}

#ifdef POLYFILL_STRCHRNUL
const char *strchrnul(
  const char *s,
  int f
) {
  while (*s && *s != f)
    ++s;
  return s;
}
#endif

struct range {
  unsigned long start, end, step;
  bool endless;
};

struct range parse_range(
  const char *str
) {
  static const char sep = ',';

  struct range range = {
    .start = 1, .end = 0,
    .step = 1,
    .endless = true
  };

# define ITERATE(body) do {                 \
    const char *adv = strchrnul(str, sep);  \
    size_t len = adv - str;                 \
                                            \
    char buf[len + 1];                      \
    memcpy(buf, str, len); buf[len] = '\0'; \
                                            \
    body;                                   \
                                            \
    if (*adv == '\0') goto done;            \
    str = adv + 1;                          \
  } while (0)
  
  ITERATE({
    if (len > 0)
      range.start = parse_int(buf);
  });

  ITERATE({
    if (len > 0) {
      range.end = parse_int(buf);
      range.endless = false;
    }
  });

  ITERATE({
    if (len == 0)
      DIE("step length cannot be empty\n");

    range.step = parse_int(buf);

    if (range.step == 0)
      DIE("step length cannot be zero\n");
    if (range.step > range.end - range.start && !range.endless)
      fprintf(stderr,
        "the step of %lu is greater than the range of values given (%lu to %lu),\n"
        "so no computations will actually be performed\n",
        range.step, range.start, range.end
      );
  });

# undef ITERATE

  if (*str != '\0')
    DIE("trailing junk '%s' in range\n", str);

  done: ;
  return range;
}



static const char *usage = "usage: %s [options] [range]\n\
-h --help          show this help\n\
\n\
-l --log           log all information to a file\n\
-t --time-format   print times with this date format\n\
                   (default %%Y/%%m/%%d %%H:%%M:%%S)\n\
-s --steps         show intermediate steps for values\n\
                   whose base-2 logarithm exceeds this value\n\
\n\
-u --before-up     run bignum reallocations before the up step\n\
-U --after-up      run bignum reallocations after the up step\n\
-d --before-down   run bignum reallocations before the down step\n\
-D --after-down    run bignum reallocations after the down step\n\
";

// investigate "suckless getopt":
// https://lobste.rs/s/pd7zaw/arg_h_suckless_alternative_getopt

int main(
  int argc,
  char **argv
) {
  u64 base = 0;

  opterr = 0;

  static struct option long_options[] = {
    { "log",         no_argument,       NULL,                        'h'  },
    { "log",         required_argument, NULL,                        'l'  },
    { "time-format", required_argument, NULL,                        't'  },
    { "steps",       optional_argument, NULL,                        's'  },
    { "before-up",   no_argument,       (int *)&realloc_before_up,   true },
    { "after-up",    no_argument,       (int *)&realloc_after_up,    true },
    { "before-down", no_argument,       (int *)&realloc_before_down, true },
    { "after-down",  no_argument,       (int *)&realloc_after_down,  true },
    { 0, 0, 0, 0 }
  };

  for (int option_index, c; (c = getopt_long(
    argc, argv,
    "h" "l:" "t:" "s::" "dDuU",
    long_options, &option_index
  )) != -1; ) {
    switch (c) {
      case 0: {
        if (long_options[option_index].flag)
          break;

        fprintf(stderr, "hit 0!?\n");
        abort();
      } break;

      case 'h': {
        fprintf(stderr, usage, *argv);
        exit(0);
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

  unsigned long step_value = 1;
  if (optind < argc) {
    struct range r = parse_range(argv[optind]);
    start_value = r.start;
    end_value = r.end;
    ending = !r.endless;
    step_value = r.step;
    ++optind;
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
  if (step_value > 1)
    printf(", step of %lu", step_value);
  printf("\n");

  printf(
    "%cO%c %cE%c\n",
    realloc_before_up ? 'Y' : 'N', realloc_after_up ? 'Y' : 'N',
    realloc_before_down ? 'Y' : 'N', realloc_after_down ? 'Y' : 'N'
  );
}
