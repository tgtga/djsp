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
