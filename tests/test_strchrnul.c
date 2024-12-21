#define _GNU_SOURCE
#include <string.h>

int main(void) {
  const char *str = "test", *res = strchrnul(str, 't');
  return !(res == str + 3);
}
