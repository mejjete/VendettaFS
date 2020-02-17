#pragma once
#include "fstart.h"

struct super_block gsuper;
struct inode_t cdir;
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
