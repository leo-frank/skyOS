#include <printf.h>
#include <syscall.h>
int main() {
  int i = 0;
  if (fork() == 0) {
    if (fork() == 0) {
      while (i <= 50) {
        printf("idle3 user process print pid %d\n", getpid());
      }
      return 0;
    }
    while (i <= 50) {
      printf("idle2 user process print pid %d\n", getpid());
    }
    return 0;
  }
  while (i <= 50) {
    printf("idle1 user process print pid %d\n", getpid());
  }
  return 0;
}