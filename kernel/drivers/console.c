#include "lock.h"
#include "log.h"
#include "sbi.h"

struct console {
  struct spinlock console_lock;
};
struct console console;
void _putchar(char ch) {
  acquire(&console.console_lock);
  sbi_console_putchar(ch);
  release(&console.console_lock);
}

char getchar(char ch) { return sbi_console_getchar(); }

void console_init() { log_init(); }