#ifndef INTERFACE
#define INTERFACE

#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include "defs.h"

#define BANNER(s) "========== " s " =========="

extern FILE *log_file;
extern char *time_format;

void message(char *, ...);

#endif
