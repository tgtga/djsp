#ifndef INTERFACE
#define INTERFACE

#include "defs.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>
#include <time.h>

#define BANNER(s) "========== " s " =========="

extern char *time_format;

void set_up_log(const char *);
void djsp_message(char *, ...);

#endif
