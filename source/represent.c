#include "../header/fstart.h"
#include "../header/fs_funct.h"
#include "../header/fs_interface.h"

void info()
{
    printf("\nGeneral Info:\n");
    printf("Super Block: [%ld] bytes\n", SUPERSIZE);
    printf("Inode Block: [%ld] bytes\n", INODESIZE);
    printf("*******************************\n");
    struct super_block super;
    vsync(&super);
    printf("Super Block:\n");
    printf("\tName: [%s]\n", super.fs_name);
    printf("\tMagic Number: [%i]\n", super.magic_number);
    printf("\tAll Space: [%ld]\n", super.all_space);

    //bitmap info
    /*
    char *bmap = (char *) malloc(80);
    dev_read(IBITMAP_START_TABLE, 80, bmap);
    printf("Inode bitmap content:\n");
    for(int i = 0; i < 80; i++)
        printf("%c", bmap[i]);
    
    dev_read(DBITMAP_START_TABLE, 80, bmap);
    printf("Data bitmap content:\n");
    for(int i = 0; i < 80; i++)
        putc(bmap[i], stdout);
    */

    //inode info
    int inode_count = 2;
    struct inode_t inode[inode_count];
    dev_read(INODE_START_TABLE, INODESIZE * inode_count, inode);
    printf("\nLOG\n");
    for(int i = 0; i < inode_count; i++)
    {
        printf("%d Inode\n", i + 1);
        printf("\tInode id:             [%d]\n", inode[i].id);
        printf("\tInode used size:      [%d]\n", inode[i].used_size);
        printf("\tInode name:           [%s]\n", inode[i].name);
        printf("\tInode block:          [%d]\n", inode[i].block[i]);
    }
}

bool module_init(const char *path)
{
    struct stat buf;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    fd = open("test.vfs", O_RDWR | O_CREAT | O_TRUNC, mode);
    //fd = creat(path, S_IRWXU | O_RDWR);
    //fd = open("test.vfs", O_RDWR | O_CREAT);
    if(fd == -1)
    {
        printf("Error creating\\opening a file\n");
        getchar();
        return false;
    }
    struct super_block test;
    test.all_space = META_BLOCKSIZE;
    test.free_space = test.all_space;
    test.magic_number = VMAGIC;
    strcpy(test.fs_name, "vendetta fs");
    
    char *temp = (char *) malloc(META_BLOCKSIZE);
    //init super_block
    write(fd, &test, SUPERSIZE);
    write(fd, temp, META_BLOCKSIZE - SUPERSIZE);    
    printf("module: Super Block Initialized\n");    
    
    //init inode bitmap block
    //availaible only 80 inode to indexing
    // size_t ibitmap = 0;
    // write(fd, &ibitmap, sizeof(size_t));
    // write(fd, temp, META_BLOCKSIZE - sizeof(size_t));
    write(fd, temp, META_BLOCKSIZE);
    printf("module: Inode Bitmap initialized\n");

    //init data bitmap block
    //availaible only 80 data block to indexing
    // size_t dbitmap = 0;
    // write(fd, &dbitmap, sizeof(size_t));
    // write(fd, temp, META_BLOCKSIZE - sizeof(size_t));
    write(fd, temp, META_BLOCKSIZE);
    printf("module: Data Bitmap initialized\n");

    //init inode table
    //availaible only 5 inodes
    struct inode_t inode;
    memset(&inode, 0, INODESIZE);
    for(int i = 0; i < 5; i++)
        write(fd, &inode, INODESIZE);
    printf("module: Inode Blocks initialized\n");

    //init first 10 data block
    for(int i = 0; i < 10; i++)
        write(fd, temp, META_BLOCKSIZE);

    //meta info
    fstat(fd, &buf);
    printf("module: File Size: [%ld]\n", buf.st_size);
    printf("module: File System Initialized\n");
    free(temp);
    return true;
}

int dev_creat(const char *file_name)
{
    srand(time(NULL));
    if(fd == 0 || file_name == NULL )
        return -1;
    if(strlen(file_name) > MAX_FILE_NAME)
    {
        printf("vcreat: file name too lagre\n");
        return -1;
    }
    if(fd == -1)
    {
        printf("vcreat: error opening file\n");
        return -1;
    }

    int i, j;
    struct inode_t inode;
    char *bitmap = (char *) malloc(80);

    //find free inode block
    dev_read(IBITMAP_START_TABLE, 80, bitmap);
    for(i = 0; i < 80; i++)
    {
        if(bitmap[i] == 0)
        {
            bitmap[i] = 1;
            break;
        }
    }
    //printf("\nvcreat: %d index are free\n", i + 1);
    dev_write(IBITMAP_START_TABLE + i, sizeof(char), &bitmap[i]);

    //find free data block
    dev_read(DBITMAP_START_TABLE, 80, bitmap);
    for(j = 0; j < 80; j++)
    {
        if(bitmap[j] == 0)
        {
            bitmap[j] = 1;
            break;
        }
    }
    printf("vcreat: %d data index are free\n", j + 1);
    dev_write(DBITMAP_START_TABLE + j, sizeof(char), &bitmap[i]);
    inode.id = (INODE_START_TABLE + (INODESIZE * i));
    inode.size = BLOCKSIZE;
    inode.used_size = 0;
    inode.cursor = inode.id;

    //set indirect block pointer
    for(int db = 0; db < INDIRECT_BLOCK_POINTER; db++)
    {
        if(inode.block[db] == 0)
        {
            inode.block[db] = DATA_START_TABLE + (BLOCKSIZE * j);
            break;
        }  
    }
    strcpy(inode.name, file_name);

    dev_write(IBITMAP_START_TABLE + j, sizeof(char), &bitmap[j]);
    dev_write(INODE_START_TABLE + (INODESIZE * i), INODESIZE, &inode);
    free(bitmap);
    return inode.id;
}

int dev_write(off_t fist_block, size_t size, void *data)
{
    lseek(fd, fist_block, SEEK_SET);
    write(fd, data, size);
    return 0;
}

int dev_read(off_t first_block, size_t size, void *dest)
{
    lseek(fd, first_block, SEEK_SET);
    read(fd, dest, size);
    return 0;
}

static inline void vsync(struct super_block *super)
{
    dev_read(0, SUPERSIZE, super);
}

int vwrite(int fd, void *buf, int count)
{
    int i = 0;
    struct inode_t inode;
    dev_read(fd, INODESIZE, &inode);
    dev_read(fd, INODESIZE, &inode);
    memset(inode.block, 0, sizeof(int) * 2);
    while(inode.block[i] != 0)
         i++;
    printf("%dth block are free\n", i);
    printf("readed file: %s\ndata block: [%d]\n", inode.name, inode.block[0]);
    inode.used_size += count;
    inode.cursor += count;
    dev_write(inode.block[i], count, buf);
    dev_write(fd, INODESIZE, &inode);
    // exit(EXIT_FAILURE);
    return 0;
}

int vread(int fd, void *buf, int count)
{
    int i;
    struct inode_t inode;
    dev_read(fd, INODESIZE, &inode);
    dev_read(inode.block[0], count, buf);
    return 0;
}

/*
void bitmap(size_t n) 
{ 
    size_t i; 
    int pos;
    for (i = 1 << 31, pos = sizeof(n); i > 0; i = i / 2, pos++) 
        (n & i)? printf("1") : printf("0"); 
    printf("\n");
} 

void to_binary(size_t num)
{
    int r;
    r = num % 2;
    if(num >= 2)
        to_binary(num / 2);
    putchar(r == 0 ? '0' : '1');
}

bool get_bitmap(size_t num, int p)
{
    if(num = (1 << p) | num)
        return true;
    else 
        return false;
}

static void set_bitmap(size_t *num, int p) 
{ 
    *num = (1 << p) | *num;
} 


void set_bit_t(unsigned *num, int pos)
{
	*num = (1 << pos) | *num;
}

void bitmap_t(unsigned num)
{
	size_t i;
	for (i = 1 << 31; i > 0; i = i / 2)
		(num & i) ? printf("1") : printf("0");
	printf("\n");
}
*/

