#include "isa.h"
#include "log.h"
#include "sched.h"
#include "strings.h"
#include "syscall_define.h"
#include "trap.h"

extern struct task_struct *current_task;

int sys_write(uint64 fd, char *buf, uint64 len) {
  char internal_buffer[256];
  if (fd == 0) {
    sstatus_set(sstatus_get() | SSTATUS_SUM);
    memcpy(internal_buffer, buf, len);
    internal_buffer[len] = '\0';
    printf("%s", internal_buffer);
    sstatus_set(sstatus_get() & ~SSTATUS_SUM);
  }
  return len;
}

uint64 sys_fork() {
  struct task_struct *p = alloc_process();
  memcpy(p->filename, current_task->filename, sizeof(current_task->filename));
  p->fpid = current_task->pid;
  p->state = TASK_RUNNING;
  memcpy(&(p->context), &(current_task->context),
         sizeof(current_task->context));
  (p->context).a0 = 0;
  (current_task->context).a0 = p->pid;
  copy_mem_from(p, current_task);
  show_process_virtual_mem_map(p);
  return (uint64)p->pid;
}

uint64 sys_getpid(struct context *t) {
  t->a0 = current_task->pid;
  return t->a0;
}

char *syscall_names[] = {
    [SYSCALL_GETPID] "getpid", [SYSCALL_FORK] "fork", [SYSCALL_WRITE] "write"};

uint64 syscall() {
  uint64 syscall_num;
  struct context *t = &(current_task->context);
  syscall_num = t->a7;
  log_debug("syscall: %s", syscall_names[syscall_num]);
  switch (syscall_num) {
    case SYSCALL_GETPID:
      return sys_getpid(t);
    case SYSCALL_FORK:
      return sys_fork();
    case SYSCALL_WRITE:
      return sys_write(t->a0, (char *)(t->a1), t->a2);
    default:
      panic("unexpected syscall number: %d", syscall_num);
  }
}