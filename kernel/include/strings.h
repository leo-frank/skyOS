#ifndef STRING_FUNCTION_H
#define STRING_FUNCTION_H

#include "type.h"

void *memset(void *dst, int c, uint n);
void *memcpy(void *dst, void *src, uint n);
void strcpy(char *dest, const char *src);
void strncpy(char *dest, const char *src, size_t n);
uint32 strlen(const char *str);
int strcmp(const char *s1, const char *s2);
char *strchr(const char *s, int c);
char *strrchr(const char *s, int c);
int tolower(int c);
int toupper(int c);
int memcmp(const void *s1, const void *s2, size_t n);

#endif /* STRING_FUNCTION_H */