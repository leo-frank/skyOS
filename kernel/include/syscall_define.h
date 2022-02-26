enum syscall_types_enum {
  SYSCALL_GETPID = 8,
  SYSCALL_FORK = 32,
  SYSCALL_WRITE = 64,
};

uint64 syscall();