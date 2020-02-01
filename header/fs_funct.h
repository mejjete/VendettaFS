#pragma once
#include <sys/types.h>
#include "fstart.h"

//device operation
int dev_read(off_t first_block, size_t size, void *dest);
int dev_write(off_t fist_block, size_t size, void *data);
static int get_unique_index();
void bitmap(size_t n);
static void set_bitmap(size_t *num, int pos);
void bin(size_t n);
void print_cur_time(struct inode_t *inode);
bool get_bitmap(size_t num, int p);
void to_binary(size_t num);

//debug information 
void info();