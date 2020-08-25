#pragma once
#include <sys/types.h>
#include <fstart.h>

//device operation
int dev_read(off_t first_block, size_t size, void *dest);
int dev_write(off_t fist_block, size_t size, void *dest);
int dev_creat(const char *path, int type, int reqsize);

/*      inode table operation       */
/* 
 * find the inode in table by given file descriptor
 * if succes returns pointer to inode, otherwise - NULL
*/
struct inode_t *find_inode(int fd);

/*
 * add inode to inode table
 * there's nothing to return
*/
void add_to_inode_table(struct inode_t *inode);

/*
 *
 * synchonization file inode with persistent
 * nothing to return
*/
void synchonize_inode_table();

void explorer();
int move_cursor(struct inode_t *inode, int cdest);
int get_free_block();
int get_free_inode();
void set_bitmap(off_t offset, char n);
void bitmap(size_t n);
void bin(size_t n);
void print_cur_time(struct inode_t *inode);

bool get_bitmap(size_t num, int p);
void to_binary(size_t num);

void bitmap_t(unsigned num);
void set_bit_t(unsigned *num, int pos);