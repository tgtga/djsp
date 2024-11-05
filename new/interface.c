#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <unistd.h>
#include <getopt.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

FILE *logfile = NULL;
#define BANNER(s) "========== " s " =========="
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

  if (logfile) {
    va_list args_log; va_copy(args_log, args_out);

    fprintf(logfile, "%s: ", time_string);
    vfprintf(logfile, format, args_log);

    va_end(args_log);
  }

  printf("%s: ", time_string);
  vprintf(format, args_out);

  va_end(args_out);
}

int main(
  int argc,
  char **argv
) {
# define DIE(...) do {                     \
    fprintf(stderr, __VA_ARGS__);          \
    fprintf(stderr, "i'm gonna die...\n"); \
    exit(1);                               \
  } while (0)

  unsigned long start = 0;
  int oneshot = 0;

  char c;
  while ((c = getopt(argc, argv, "l:n:t:o")) != -1) {
    switch (c) {
      case 'l': {
        if ((logfile = fopen(optarg, "a")) == NULL)
          DIE("unable to open file `%s': %s\n", optarg, strerror(errno));

        setvbuf(logfile, NULL, _IOLBF, 0);

        message("hello, world! :3\n");
      break; }

      case 'n': {
        errno = 0;
        char *end;
        start = strtoul(optarg, &end, 0);

        if (errno)
          DIE("unable to convert `%s' to an integer: %s\n", optarg, strerror(errno));

        if (*end != '\0') {
          if (end == optarg)
            DIE("unable to find an integer in `%s'\n", optarg);
          else
            fprintf(stderr, "trailing junk `%s' in `%s'\n", end, optarg);
        }

        if (end == optarg)
          DIE("empty integer given\n");

        printf("your number is %lu\n", start);
      break; }

      case 't': {
        time_format = optarg;
      break; }

      case 'o': {
        printf("niko oneshot...\n");
        oneshot = 1;
      break; }

      case '?': {
        switch (optopt) {
          case 'l':
          case 'n':
          case 't': {
            fprintf(stderr, "no issue here!\n");
            goto next_option;
          break; }
        }

        fprintf(stderr, "optarg is `%s', optopt is `%c'... whar...?\n", optarg, optopt);
      break; }

      default: DIE("oh nooo! default hit!\n");

      next_option: ;
    }
  }
}
