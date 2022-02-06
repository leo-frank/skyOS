#include "isa.h"
#include "log.h"
#include "sched.h"
#include "strings.h"
#include "trap.h"

extern struct task_struct *current_task;

int sys_write(uint64 fd, char *buf, uint64 len) {
  char c;
  if (fd == 0) {
    sstatus_set(sstatus_get() | SSTATUS_SUM);
    memcpy(&c, buf, 1);
    printf("%c", c);
    sstatus_set(sstatus_get() & ~SSTATUS_SUM);
  }
  return 1;
}

struct task_struct *sys_fork() {
  struct task_struct *p = alloc_process();
  memcpy(p->filename, current_task->filename, sizeof(current_task->filename));
  p->fpid = current_task->pid;
  p->state = TASK_RUNNING;
  memcpy(&(p->context), &(current_task->context),
         sizeof(current_task->context));
  (p->context).a0 = 0;
  (current_task->context).a0 = p->pid;
  copy_mem_from(p, current_task);
  return p;
}

void sys_getpid(struct context *t) { t->a0 = current_task->pid; }

void syscall(struct context *t) {
  // ld	a3,-128(s0)
  // 切換棧指針
  uint64 syscall_num;

  syscall_num = t->a7;
  if (syscall_num == 64) {
    uint64 fd = t->a0;
    char *buf = (char *)(t->a1);
    uint64 len = t->a2;
    sys_write(fd, buf, len);
  } else if (syscall_num == 32) {
    sys_fork();
  } else if (syscall_num == 8) {
    sys_getpid(t);
  } else {
    panic("unexpected syscall number: %d", syscall_num);
  }
}