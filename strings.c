#include "type.h"
void *memset(void *dst, int c, uint n) {
  char *cdst = (char *)dst;
  int i;
  for (i = 0; i < n; i++) {
    cdst[i] = c;
  }
  return dst;
}
void *memcpy(void *dst, void *src, uint n) {
  char *cdst = (char *)dst;
  char *csrc = (char *)src;
  int i;
  for (i = 0; i < n; i++) {
    cdst[i] = csrc[i];
  }
  return dst;
}