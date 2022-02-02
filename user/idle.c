#include <printf.h>
#include <syscall.h>
int main() {
  int i = 0;
  while (i <= 50) {
    printf("idle user process print %d\n", i);
  }
  return 0;
}