#pragma once
#include "fstart.h"

struct super_block gsuper;
struct inode_t cdir;
char current_path[50];
int fd;

int fs_create(const char *fsname);
bool module_init(const char *path);
bool module_exit();
static inline void vsync(struct super_block *super);
int vcreat(const char *file_name);
int vread(int fd, void *buf, int count);
int vwrite(int fd, void *buf, int count);
int vseek(int fd, off_t offset, int whence);
int vopen(const char *file_name);
int vremove(const char *file_name);
int change_dir(const char *dir_name);
int look_dir();
int ucat(const char *file_name);
int uwrite(const char *file_name, char *text);
