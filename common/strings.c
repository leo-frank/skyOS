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

void strcpy(char *dest, const char *src) {
  while (*src) {
    *(dest++) = *(src++);
  }
  *dest = 0;
}

void strncpy(char *dest, const char *src, size_t n) {
  while (n-- && *src) {
    *(dest++) = *(src++);
  }
  *dest = 0;
}

uint32 strlen(const char *str) {
  uint32 i = 0;
  while (str[i]) i++;
  return i;
}

int strcmp(const char *s1, const char *s2) {
  while (*s1 == *s2) {
    if (!*(s1++)) {
      return 0;  // Null terminator
    }

    s2++;
  }
  return (*s1) - *(s2);
}

// strchr - Get pointer to first occurance of c in string s
char *strchr(const char *s, int c) {
  while (*s != (char)c)
    if (!*s++) return NULL;
  return (char *)s;
}

// strrchr - Get pointer to last occurance of c in string s
char *strrchr(char *s, int c) {
  char *occ = NULL;
  while (*s)
    if (*s++ == c) occ = s;
  return occ;
}

int tolower(int c) {
  if ((c >= 'A') && (c <= 'Z')) return c + ('a' - 'A');
  return c;
}

int toupper(int c) {
  if ((c >= 'a') && (c <= 'z')) return c + ('A' - 'a');
  return c;
}

int memcmp(const void *s1, const void *s2, size_t n) {
  const uint8 *a = (uint8 *)s1;
  const uint8 *b = (uint8 *)s2;

  for (size_t i = 0; i < n; i++) {
    if (a[i] < b[i]) {
      return -1;
    } else if (a[i] > b[i]) {
      return 1;
    }
  }

  return 0;
}