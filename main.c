#include "log.h"
#include "logo.h"
#include "pmm.h"
#include "trap.h"
#include "vms.h"
extern void move_to_user_mode();
void main() {
  logo();
  log_set_level(LOG_DEBUG);
  pmm_init();
  vms_init();
  trap_init();
  move_to_user_mode();
}