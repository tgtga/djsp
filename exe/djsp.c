#define _GNU_SOURCE 1

#include <stdio.h>
#include <getopt.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "../include/defs.h"
#include "../include/logging.h"
#include "../include/oneshot.h"
#include "../include/sequence.h"



FILE *open_file(
  char *path
) {
  FILE *out = fopen(path, "a");
  if (!out)
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

void parse_range(
  const char *str,
  unsigned long *start_out, unsigned long *end_out,
  unsigned long *step_out,
  bool *endless_out
) {
  // FIXME: recognize bare number as oneshot range

  static const char sep = ',';

  unsigned long
    start = 1, end = 0,
    step = 1;
  bool endless = true;

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
      start = parse_int(buf);
  });

  ITERATE({
    if (len > 0) {
      end = parse_int(buf);
      endless = false;
    }
  });

  ITERATE({
    if (len == 0)
      DIE("step length cannot be empty\n");

    step = parse_int(buf);

    if (step == 0)
      DIE("step length cannot be zero\n");
    if (step > end - start && !endless)
      fprintf(stderr,
        "the step of %lu is greater than the range of values given (%lu to %lu),\n"
        "so no computations will actually be performed\n",
        step, start, end
      );
  });

# undef ITERATE

  if (*str != '\0')
    DIE("trailing junk '%s' in range\n", str);

  done: ;
  *start_out   = start;
  *end_out     = end;
  *step_out    = step;
  *endless_out = endless;
}



static const char *usage = "usage: %s [options] [range] [base]\n\
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
# if 0
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

      // case 'l': log_file = open_file(optarg); break;

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

  unsigned long start = 1, end = 0, step = 1; bool endless = true;
  if (optind < argc) {
    parse_range(argv[optind], &start, &end, &step, &endless);
    ++optind;
  }

  u64 base = 0;
  if (optind < argc) {
    base = parse_int(argv[optind]);
    ++optind;
  }

  if (optind < argc)
    fprintf(stderr, "ignoring trailing arguments...\n");

  // message("hello, world!\n");

  if (!show_steps) {
    fprintf(stderr, "NOT showing steps!");
  } else {
    fprintf(stderr, "showing steps");
    if (ssol)
      fprintf(stderr, " above 2^%lu", ssol);
  }
  fprintf(stderr, "\n");

  fprintf(stderr, "running on %lu", start);
  if (endless)
    fprintf(stderr, " -> oo");
  else
    if (end != start)
      fprintf(stderr, " -> %lu", end);
  if (step > 1)
    fprintf(stderr, ", step of %lu", step);
  if (base)
    fprintf(stderr, ", base %lu", base);
  fprintf(stderr, "\n");

  fprintf(stderr, 
    "%cO%c %cE%c\n",
    realloc_before_up ? 'Y' : 'N', realloc_after_up ? 'Y' : 'N',
    realloc_before_down ? 'Y' : 'N', realloc_after_down ? 'Y' : 'N'
  );
  
  if (start == end) {
    u64 where = start;

    printf("L");
    if (base)
      printf("_%lu", base);
    printf("(%lu) = ", where);
    fflush(stdout);

    u64 r = base ? oneshot_n(where, base, NULL) : oneshot_2(where, NULL);
    printf("%lu\n", r);
  } else {
    // sequence(base, start, end, step, endless);
  }
# endif
}
