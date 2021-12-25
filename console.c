#include "sbi.h"
void _putchar(char ch) {
    sbi_console_putchar(ch);
}
char getchar(char ch) {
    return sbi_console_getchar();
}