#include "printf.h"
void panic(char *s) {
    printf(s);
    while(1);
}