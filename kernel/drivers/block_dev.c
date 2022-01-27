/*
 * This file is part of the openfat project.
 *
 * Copyright (C) 2011  Department of Physics, University of Otago
 * Written by Gareth McMullin <gareth@blacksphere.co.nz>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* Sample block device implementation:
 * Implementation of abstract block device over a Unix file
 */

#define _FILE_OFFSET_BITS 64

#include "openfat/blockdev.h"
#include "pmm.h"
#include "ramdisk.h"

#define FILE_SECTOR_SIZE 512

struct block_device *block_device_new() {
  struct block_device *bldev;

  bldev = kalloc(sizeof(*bldev));

  bldev->read_sectors = ramdisk_read;
  bldev->write_sectors = ramdisk_write;

  return (struct block_device *)bldev;
}

void block_device_destroy(struct block_device *bldev) {
  // TODO: enhance kfree argument settings
  // kfree(bldev);
}
