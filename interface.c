/*
time_format = "%Y/%m/%d %H:%M:%S"
global_log : IO? = nil
def log(
  where : IO,
  format : String,
  *printf_args
) : Nil
  strtime = Time.local.to_s(time_format) + ": "

  where.print strtime
  where.printf format, *printf_args
  where.flush

  unless global_log.nil?
    global_log.print strtime
    global_log.printf format, *printf_args
    global_log.flush
  end
end
*/

char *time_format = "%Y/%m/%d %H:%M:%S";
FILE *global_log = NULL;

void log(
  char *format,
  ...
) {
  time_t tm = time(NULL);
  struct tm *p = localtime(&tm);

  char time_string[65];
  strftime(time_string, sizeof(time_string), time_format, p);

  printf("%s", time_string);
  printf
}

void print_time(
  const char *separator,
  size_t count,
  ...
) {
  time_t tm = time(NULL);
  struct tm *p = localtime(&tm);

  char buffer[4 + 1 + 2 + 1 + 2 + 1 + 2 + 1 + 2 + 1 + 2 + 1];
  strftime(buffer, sizeof(buffer), "%Y/%m/%d %H:%M:%S", p);

  va_list files;
  va_start(files, count);

  for (size_t pos = 0; pos < count; ++pos) {
    FILE *current_file = va_arg(files, FILE *);
    fprintf(current_file, "%s%s", buffer, separator);
  }

  va_end(files);
}
