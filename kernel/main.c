#include "log.h"
#include "logo.h"
#include "openfat.h"
#include "pmm.h"
#include "ramdisk.h"
#include "trap.h"
#include "vms.h"

extern void proctest();

void main() {
  log_set_level(LOG_INFO);
  logo();
  pmm_init();
  vms_init();
  trap_init();
  ramdisk_init();
  filesystem_init();
  proctest();
}