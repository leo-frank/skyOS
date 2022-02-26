#define O_ACCMODE 0003

enum file_open_flags_enum {
  O_RDONLY = 00,
  O_WRONLY = 01,
  O_RDWR = 02,
  O_CREAT = 0100,
  O_APPEND = 02000,
};

enum seek_positions_enum {
  SEEK_SET = 0, /* Seek from beginning of file.  */
  SEEK_CUR = 1, /* Seek from current position.  */
  SEEK_END = 2, /* Seek from end of file.  */
};