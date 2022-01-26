#include "log.h"
void panic(char *s) {
  log_error(s);
  while (1)
    ;
}