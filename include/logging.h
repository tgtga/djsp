#ifndef INTERFACE
#define INTERFACE

#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <unistd.h>
#include "defs.h"

#define BANNER(s) "========== " s " =========="

extern char *time_format;

void set_up_log(const char *);
void message(char *, ...);

#endif
