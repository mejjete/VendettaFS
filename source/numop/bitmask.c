
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <stdbool.h>
#include <bitmask.h>
#include <fstart.h>
#include <fs_interface.h>

void set_bit(uint8_t *n, uint16_t bit) { *n |= 1 << bit; };
void unset_bit(uint8_t *n, uint16_t bit) { *n |= 0 << bit; };

vfs_bitmask create_bitmask(int size)
{
    vfs_bitmask mask;
    mask.bitmask = (uint8_t *) malloc(size / 8);
    memset(mask.bitmask, 0, size / 8);
    mask.size = size;
    return mask;
};

void release_bitmask_block(int data_block, vfs_bitmask *bmask)
{   
    uint8_t *ptr = bmask->bitmask;
    int pos = (int)data_block / 8;
    int bit = data_block - (pos * 8);
    unset_bit(&ptr[pos], bit);
    if(bmask == &_vfs_super_block.__Data_bitmask_table)
        _vfs_super_block.free_data_blocks++;
    else 
        _vfs_super_block.free_inode_blocks++;
}