#include <fstart.h>
#include <fs_funct.h>
#include <fs_interface.h>
#include <internal_debug.h>

void info()
{
    #ifdef NDEBUG
        printf("\nGeneral Info:\n");
        printf("Super Block: [%ld] bytes\n", SUPERSIZE);
        printf("Inode Block: [%ld] bytes\n", INODESIZE);
        printf("*******************************\n");
        struct super_block super;
        fs_sync(&super);
        printf("Super Block:\n");
        printf("\tName: [%s]\n", super.fs_name);
        printf("\tMagic Number: [%i]\n", super.magic_number);
        printf("\tAll Space: [%ld]\n", super.all_space);
    #endif

    //inode info
    int inode_count = 5;
    struct inode_t inode[inode_count];
    dev_read(INODE_START_TABLE, INODESIZE * inode_count, inode);
    printf("\nLOG\n");
    for(int i = 0; i < inode_count; i++)
    {
        printf("%d Inode\n", i + 1);
        printf("\tInode id:             [%d]\n", inode[i].id);
        printf("\tInode used size:      [%d]\n", inode[i].used_size);
        printf("\tInode size:           [%d]\n", inode[i].size);
        printf("\tInode name:           [%s]\n", inode[i].name);
        printf("\tInode parent:         [%d]\n", inode[i].parent);
        if(inode[i].type == VFILE) 
            printf("\tInode blocks:       ");
        else 
            printf("\tInode file blocks:  ");
        for(int j = 0; j < 5; j++)
            printf("  [%d] ", inode[i].block[j]);
        printf("\n");
        if(inode[i].type == VFILE)
            printf("\tInode is file\n");
        else if(inode[i].type == VDIR)
            printf("\tInode is directory\n");
        else 
            printf("\tInode is something else\n");
    }
}

void node_struct(struct inode_t *inode)
{
	printf("---------------------------------------\n");
	printf("Adress: %d\n", inode->id);
    printf("Parent: %d\n", inode->parent);
	printf("Cursor: %d\n", inode->cursor);
	printf("Used size: %d\n", inode->used_size);
	printf("General size: %d\n", inode->size);
}

void show_inode_bitmap()
{
    printf("---INODE BITMAP--- ---DATA BITMAP---\n");
    char ibitmap[80], dbitmap[80];
    dev_read(IBITMAP_START_TABLE, 80, &ibitmap);
    dev_read(DBITMAP_START_TABLE, 80, &dbitmap);
    for(int c = 0; c < 4; c++)
    {
        for(int i = 0; i < 5; i++)
            printf("%3d", ibitmap[(c * 5) + i]);
        printf("  |");
        for(int j = 0; j < 5; j++)
            printf("%3d", dbitmap[(c * 5) + j]);
        printf("  |\n");
    }
    printf("------------------ -----------------\n");
}

static inline void fs_sync(struct super_block *super)
{
    dev_read(0, SUPERSIZE, super);
}