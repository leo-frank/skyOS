
struct fat_file_handle {
  struct fat_vol_handle *fat;
  /* Fields from dir entry */
  uint32 size;
  uint32 first_cluster;
  /* Internal state information */
  uint32 position;    /* current write & read position(bytes) */
  uint32 cur_cluster; /* This is used for sector on FAT12/16 root */
  uint8 root_flag;    /* Flag to mark root directory on FAT12/16 */
  int flags;
  /* Reference to dirent */
  uint32 dirent_sector;
  uint16 dirent_offset;
};

fat_read:
1.根据h->position和h->cur_cluster得到当前正要读的sector和offset
2.根据offset计算这轮要读的bytes，记为chunk。
3.开始读，调用FAT_GET_SECTOR函数，该函数会根据sector来读某个扇区。每次读chunk bytes。之后更新h->position。
4.如果sector是h->fat->sectors_per_cluster的倍数，说明我们应该读下一个cluster的内容了。通过_fat_get_next_cluster访问fat表得到下一个cluster的序号，更新为h->cur_cluster。
5.更新sector
