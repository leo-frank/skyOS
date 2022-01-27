#include "log.h"
#include "pmm.h"
#include "strings.h"
#include "type.h"

extern char kernel_start[];

// works only with kernel ram size less than 256MiB
#define SECTOR_SIZE 512

// use char * not uint64*, which has no potential risk when add integer
char *RAMDISK_BASE = (char *)((uint64)kernel_start + mem_size / 2);

void ramdisk_init() { log_info("ramdisk addr: 0x%x", RAMDISK_BASE); }

// read count size from src sector
int ramdisk_read(void *buf, uint32 sector, uint32 count) {
  memcpy(buf, RAMDISK_BASE + sector * SECTOR_SIZE, count * SECTOR_SIZE);
  return count;
}

// write count size to dst sector
int ramdisk_write(void *src, uint32 sector, uint32 count) {
  memcpy(RAMDISK_BASE + sector * SECTOR_SIZE, src, count * SECTOR_SIZE);
  return count;
}
