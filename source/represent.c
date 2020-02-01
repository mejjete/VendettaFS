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
    printf("\tAll Space: [%d]\n", super.all_space);

    struct inode_t inode[2];
    dev_read(INODE_START_TABLE, INODESIZE * 2, inode);
    printf("LOG\n");
    //inode info
    for(int i = 0; i < 2; i++)
    {
        printf("%d Inode\n", i + 1);
        printf("\tInode id:     [%d]\n", inode[i].id);
        printf("\tInode size:   [%d]\n", inode[i].size);
        printf("\tInode name:   [%s]\n", inode[i].name);
    }
}

void vsync(struct super_block *super)
{
    lseek(fd, 0, SEEK_SET);
    read(fd, super, SUPERSIZE);
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
    test.all_space = 4096;
    test.free_space = 4096 - SUPERSIZE;
    test.magic_number = VMAGIC;
    strcpy(test.fs_name, "vendetta fs");
    //printf("module: Super Block Initialized\n");
    
    char temp[KBYTE * 4];
    //init super_block
    write(fd, &test, SUPERSIZE);
    write(fd, temp, 4096 - SUPERSIZE);    
    printf("module: Super Block Initialized\n");    
    
    //init inode and data bitmap block
    write(fd, temp, 4096);
    write(fd, temp, 4096);
    printf("module: Inode/data bitmap initialized\n");

    //init inode table
    struct inode_t inode;
    memset(&inode, 0, INODESIZE);
    for(int i = 0; i < 5; i++)
        write(fd, &inode, INODESIZE);
    printf("module: Inode blocks initialized\n");

    //
    fstat(fd, &buf);
    printf("module: File Size: [%ld]\n", buf.st_size);
    // free(temp);
    // write(fd, &test, sizeof(struct super_block));
    printf("module: File System Initialized\n");
    return true;
}

int vcreat(const char *file_name)
{
    srand(time(NULL));
    if(fd == 0 || file_name == NULL )
        return -1;
    if(strlen(file_name) > MAX_FILE_NAME)
    {
        printf("vcreat: file name too lagre\n");
        return -1;
    }
    struct super_block super;
    if(fd == -1)
    {
        printf("vcreat: error opening file\n");
        return -1;
    }

    struct inode_t inode[5];
    dev_read(INODE_START_TABLE, INODESIZE * 5, inode);
    // read(fd, inode, INODESIZE * 5);
    for(int i = 0; i < 5; i++)
    {   
        if(inode[i].id == 0)
        {
            inode[i].id = rand();
            inode[i].size = BLOCKSIZE;
            strcpy(inode[i].name, file_name);
            dev_write(INODE_START_TABLE + (INODESIZE * i), INODESIZE, &inode[i]);
            return 0;
        }
    }
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

static void bitmap(unsigned n) 
{ 
    unsigned i; 
    int pos;
    for (i = 1 << 31, pos = sizeof(n); i > 0; i = i / 2, pos++) 
        (n & i)? printf("%d bits is 1\n", pos) : pos + 0; 
} 
