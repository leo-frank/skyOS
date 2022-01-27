#pragma once

#include "type.h"

void ramdisk_init();
int ramdisk_read(void *dst, uint32 sector_start, uint32 nsector);
int ramdisk_write(const void *src, uint32 sector_start, uint32 nsector);