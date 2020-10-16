#pragma once
#include <sys/types.h>
#include <fstart.h>
#include <bitmask.h>

//device operation
int dev_read(off_t first_block, size_t size, void *dest);
int dev_write(off_t fist_block, size_t size, void *dest);
int dev_creat(const char *path, int type, int reqsize);

void explorer();
int move_cursor(struct inode_t *inode, int cdest);
int get_free_block();
int get_free_inode();
void set_bitmap(off_t offset, char n);
void print_cur_time(struct inode_t *inode);