#pragma once
#include <sys/types.h>
#include "fstart.h"

//device operation
int dev_read(off_t first_block, size_t size, void *dest);
int dev_write(off_t fist_block, size_t size, void *dest);
int dev_creat(const char *path, int type, int reqsize);

int move_cursor(struct inode_t *inode, int cdest);
int get_free_block();
int get_free_inode();
void bitmap(size_t n);
static void set_bitmap(size_t *num, int pos);
void bin(size_t n);
void print_cur_time(struct inode_t *inode);

bool get_bitmap(size_t num, int p);
void to_binary(size_t num);


void bitmap_t(unsigned num);
void set_bit_t(unsigned *num, int pos);

//debug information 
void info();