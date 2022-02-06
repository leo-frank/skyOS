.global write
write:
  li a7, 64
  ecall
  ret
.global fork
fork:
  li a7, 32
  ecall
  ret
.global execve
execve:
  li a7, 16
  ecall
  ret
.global getpid
getpid:
  li a7, 8
  ecall
  ret