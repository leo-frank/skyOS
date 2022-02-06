#include "log.h"
#include "logo.h"
#include "openfat.h"
#include "pmm.h"
#include "ramdisk.h"
// #include "sched.h"
#include "trap.h"
#include "vms.h"

extern void execve(char *);

void end();

void main() {
  log_set_level(LOG_INFO);
  logo();
  pmm_init();
  vms_init();
  trap_init();
  ramdisk_init();
  filesystem_init();
  // log_set_level(LOG_DEBUG);
  execve("idle");
  end();
}

void end() {
  log_info("execute to end of OS ...");
  log_info("a forever loop start ...");
  while (1)
    ;
}