#include <stdio.h>
#include <getopt.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "interface.c"
#include "seq.c"
#include "defs.c"

unsigned long start = 0;
void set_start(
  char *str
) {
  start = parse_int(str, "-n");
}

int oneshot = 0;
void set_oneshot(void) { oneshot = 1; }

int main(
  int argc,
  char **argv
) {
  char c;
  while ((c = getopt(argc, argv, "l:n:t:s:o")) != -1) {
    switch (c) {
      case 'l': {
        set_log_file(optarg);

        message("hello, world! :3\n");
      break; }

      case 'n': {
        set_start(optarg);
        
        printf("your number is %lu\n", start);
      break; }

      case 't': set_time_format(optarg); break;

      case 's': {
        set_ssol(optarg);

        printf("showing steps over 2**%lu now!\n", ssol);
      break; }

      case 'o': {
        set_oneshot();

        printf("niko oneshot...\n");
      break; }

      case '?': {
        switch (optopt) {
          case 'l':
          case 'n':
          case 't':
          case 's': {
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
