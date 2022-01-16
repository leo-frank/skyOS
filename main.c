#include "log.h"
#include "logo.h"
#include "pmm.h"
#include "vms.h"

void main() {
  log_set_level(LOG_INFO);
  logo();
  pmm_init();
  vms_init();
}