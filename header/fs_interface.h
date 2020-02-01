#pragma once
#include "fstart.h"

static struct super_block gsuper;
static int fd;

bool module_init(const char *path);
void vsync(struct super_block *super);
int vcreat(const char *file_name);