#include "assert.h"
#include "fcntl.h"
#include "log.h"
#include "openfat.h"
#include "printf.h"
#include "strings.h"
#include "type.h"
extern struct block_device *block_device_new(const char *filename,
                                             const char *mode);
extern void block_device_destroy(struct block_device *bldev);

FatVol vol;

void print_tree(struct fat_vol_handle *vol, struct fat_file_handle *dir,
                const char *path) {
  struct dirent ent;
  char tmppath[1024];
  struct fat_file_handle subdir;

  while (!fat_readdir(dir, &ent)) {
    if ((strcmp(ent.d_name, ".") == 0) || (strcmp(ent.d_name, "..") == 0))
      continue;
    sprintf(tmppath, "%s/%s", path, ent.d_name);
    log_info(tmppath);

    if (ent.fat_attr == FAT_ATTR_DIRECTORY) {
      fat_chdir(vol, ent.d_name);
      assert(fat_open(vol, ".", 0, &subdir) == 0);
      print_tree(vol, &subdir, tmppath);
      fat_chdir(vol, "..");
    }
  }
}

#define FAT_IMAGE "fat12.img"

void fstest() {
  struct block_device *bldev;
  FatFile file;
  char *rootpath = "/";

  bldev = block_device_new(FAT_IMAGE, "r+");
  assert(bldev != NULL);

  assert(fat_vol_init(bldev, &vol) == 0);
  log_info("Fat type is FAT%d\n", vol.type);

  fat_mkdir(&vol, "Directory1");
  fat_mkdir(&vol, "Directory2");
  fat_mkdir(&vol, "Directory3");
  assert(fat_chdir(&vol, "Directory1") == 0);
  fat_mkdir(&vol, "Directory1");
  fat_mkdir(&vol, "Directory2");
  fat_mkdir(&vol, "Directory3");
  if (fat_create(&vol, "Message file with a long name.txt", O_WRONLY, &file) ==
      0) {
    for (int i = 0; i < 100; i++) {
      char message[80];
      sprintf(message, "Here is a message %d\n", i);
      assert(fat_write(&file, message, strlen(message)) ==
             (int)strlen(message));
    }
  }
  assert(fat_chdir(&vol, "..") == 0);
  assert(fat_open(&vol, ".", O_RDONLY, &file) == 0);
  print_tree(&vol, &file, rootpath[0] == '/' ? rootpath + 1 : rootpath);

  assert(fat_open(&vol, "idle", O_RDONLY, &file) == 0);

  block_device_destroy(bldev);
}

void filesystem_init() {
  fstest();
  struct block_device *bldev;

  bldev = block_device_new(FAT_IMAGE, "r+");
  assert(fat_vol_init(bldev, &vol) == 0);

  log_info("Fat type is FAT%d\n", vol.type);

  // FIXME: any code for destroy this device ?
}