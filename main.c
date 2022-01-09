#include "log.h"
#include "logo.h"
#include "pmm.h"
#include "vms.h"

void main() {
  logo();
  pmm_init();
  log_trace("Hello %s", "world");
  log_debug("Hello %s", "world");
  log_info("Hello %s", "world");
  log_warn("Hello %s", "world");
  log_error("Hello %s", "world");
  log_fatal("Hello %s", "world");
}