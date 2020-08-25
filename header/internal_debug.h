//file for getting debug information
#ifndef INTERNAL_DEBUG_H
    #define INTERNAL_DEBUG_H
#define NDEBUG

//general debug info
void info();

//show internal struct of particular inode
void node_struct(struct inode_t *inode);

//print first 20 blocks of inode and data bitmap
void show_inode_bitmap();

//synhronize super block with struct
static inline void fs_sync(struct super_block *super);

//show inode table
void show_inode_table();

#endif