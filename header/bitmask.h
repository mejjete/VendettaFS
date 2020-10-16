#ifndef VFS_BITMASK_NUMBER_OP   
    #define VFS_BITMASK_NUMBER_OP
#include <limits.h>

#define BITSIZE CHAR_BIT

typedef struct //__vfs__bitmask
{
    uint8_t *bitmask;
    int size;
}vfs_bitmask;

//size is an amount of BITS you need
vfs_bitmask create_bitmask(int size);

/*
 * data_block - index of block
 * bitmask - pointer to bitmask which data_block related to
*/
void release_bitmask_block(int data_block, vfs_bitmask *bitmask);

void set_bit(uint8_t *n, uint16_t bit);
void unset_bit(uint8_t *n, uint16_t bit);
#endif