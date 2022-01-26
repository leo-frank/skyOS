#include "log.h"
#include "logo.h"
#include "pmm.h"
#include "ramdisk.h"
#include "trap.h"
#include "vms.h"
extern void move_to_user_mode();
extern void fstest();
void main() {
  log_set_level(LOG_INFO);
  logo();
  pmm_init();
  vms_init();
  trap_init();
  ramdisk_init();
  fstest();
  // move_to_user_mode();
}