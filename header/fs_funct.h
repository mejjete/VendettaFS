#pragma once
#include <sys/types.h>

//device operation
int dev_read(off_t first_block, size_t size, void *dest);
int dev_write(off_t fist_block, size_t size, void *data);
static int get_unique_index();
static void bitmap(unsigned n);

//debug information 
void info();