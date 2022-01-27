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

/** \file blockdev.h
 * \brief Block device abstraction.
 * Must be implemented by application for a physical device.
 */

#ifndef __BLOCKDEV_H
#define __BLOCKDEV_H

#include "type.h"

/** \brief Structure representing an abstract block device.
 * This abstraction must be provided by the application.
 */
struct block_device {
  /** \brief Method to read sectors. */
  int (*read_sectors)(void *buf, uint32 sector, uint32 count);
  /** \brief Method to write sectors. */
  int (*write_sectors)(const void *buf, uint32 sector, uint32 count);
};

/* Returns the number of sectors read or negative on error */
static inline int __attribute__((warn_unused_result))
block_read_sectors(const struct block_device *dev, uint32 sector, uint32 count,
                   void *buf) {
  return dev->read_sectors(buf, sector, count);
}

/* Returns the number of sectors written or negative on error */
static inline int __attribute__((warn_unused_result))
block_write_sectors(const struct block_device *dev, uint32 sector, uint32 count,
                    const void *buf) {
  return dev->write_sectors(buf, sector, count);
}
#endif
