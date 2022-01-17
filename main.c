#include "log.h"
#include "logo.h"
#include "pmm.h"
#include "trap.h"
#include "vms.h"

void main() {
  logo();
  log_set_level(LOG_DEBUG);
  pmm_init();
  vms_init();
  trap_init();
}