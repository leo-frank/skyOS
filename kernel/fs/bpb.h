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

/* Boot Sector / BIOS Parameter Block definitions.
 * Convenience functions for calculations.
 */

#ifndef __BPB_H
#define __BPB_H

#include "openfat/leaccess.h"

/* Boot sector fields common to FAT12/FAT16/FAT32 */
struct bpb_common {
  uint8 boot_jmp[3];
  char oem_name[8];
  uint16 bytes_per_sector;
  uint8 sectors_per_cluster;
  uint16 reserved_sector_count;
  uint8 num_fats;
  uint16 root_entry_count;
  uint16 total_sectors_16;
  uint8 media;
  uint16 fat_size_16;
  uint16 sectors_per_track;
  uint16 num_heads;
  uint32 hidden_sectors;
  uint32 total_sectors_32;
} __attribute__((packed));

/* Boot sector fields only in FAT12/FAT16 */
struct bpb_fat12_16 {
  struct bpb_common common;
  uint8 drive_num;
  uint8 Reserved1;
  uint8 boot_sig;
  uint32 volume_id;
  char volume_label[11];
  char fs_type[8];
} __attribute__((packed));

/* Boot sector fields only in FAT32 */
struct bpb_fat32 {
  struct bpb_common common;
  uint32 fat_size_32;
  uint16 ext_flags;
  uint16 fs_version;
  uint32 root_cluster;
  uint16 fs_info;
  uint16 bk_boot_sec;
  uint8 Reserved[12];
  uint8 drive_num;
  uint8 Reserved1;
  uint8 boot_sig;
  uint32 volume_id;
  char volume_label[11];
  char fs_type[8];
} __attribute__((packed));

static inline uint32 _bpb_root_dir_sectors(struct bpb_common *bpb) {
  return ((__get_le16(&bpb->root_entry_count) * 32) +
          (__get_le16(&bpb->bytes_per_sector) - 1)) /
         __get_le16(&bpb->bytes_per_sector);
}

static inline uint32 _bpb_fat_size(struct bpb_common *bpb) {
  uint32 fat_size = __get_le16(&bpb->fat_size_16);
  if (fat_size == 0)
    fat_size = __get_le32(&((struct bpb_fat32 *)bpb)->fat_size_32);

  return fat_size;
}

static inline uint32 _bpb_first_data_sector(struct bpb_common *bpb) {
  return __get_le16(&bpb->reserved_sector_count) +
         (bpb->num_fats * _bpb_fat_size(bpb)) + _bpb_root_dir_sectors(bpb);
}

static inline uint32 _bpb_first_sector_of_cluster(struct bpb_common *bpb,
                                                  uint32 n) {
  return ((n - 2) * bpb->sectors_per_cluster) + _bpb_first_data_sector(bpb);
}

enum fat_type {
  FAT_TYPE_FAT12 = 12,
  FAT_TYPE_FAT16 = 16,
  FAT_TYPE_FAT32 = 32,
};

static inline uint32 _bpb_cluster_count(struct bpb_common *bpb) {
  uint32 tot_sec = __get_le16(&bpb->total_sectors_16);
  if (tot_sec == 0) tot_sec = __get_le32(&bpb->total_sectors_32);

  uint32 data_sec = tot_sec - __get_le16(&bpb->reserved_sector_count) -
                    (bpb->num_fats * _bpb_fat_size(bpb)) -
                    _bpb_root_dir_sectors(bpb);

  return data_sec / bpb->sectors_per_cluster;
}

/* FAT type is determined by count of clusters */
static inline enum fat_type fat_type(struct bpb_common *bpb) {
  uint32 cluster_count = _bpb_cluster_count(bpb);
  if (cluster_count < 4085) {
    return FAT_TYPE_FAT12;
  } else if (cluster_count < 65525) {
    return FAT_TYPE_FAT16;
  }
  return FAT_TYPE_FAT32;
}

#endif
