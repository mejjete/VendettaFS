#pragma once
#include "fstart.h"

static struct super_block gsuper;
static int fd;

int fs_create(const char *fsname);
bool module_init(const char *path);
static inline void vsync(struct super_block *super);
int vcreat(const char *file_name);
int vread(int fd, void *buf, int count);
int vwrite(int fd, void *buf, int count);
int vopen(const char *file_name);
