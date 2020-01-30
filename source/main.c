#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include "../header/fstart.h"

#define BYTE 1
#define KBYTE BYTE * 1024
#define MBYTE KBYTE * 1024

#define SUPERSIZE sizeof(struct super_block)
#define INODESIZE sizeof(struct inode_part)
#define INODECOUNT 15
#define BLOCKSIZE 4096

#define MAX_FILE_NAME 32
#define MAX_DIRECTORY_NAME 16 

//FILE DEFINED MACROS
#define FILE_DEFAULT_SIZE KBYTE
//is open
#define FILE_OPEN   0x67
#define FILE_CLOSE  0x55
//file mode
#define FILE_NULL   0x70
#define FILE_RONLY  0x80
#define FILE_WONLY  0x90   

struct super_block;
void vsync(struct super_block *super);

struct
{
    int fdesk;
}general_info;

struct inode_part
{
    int ufi;
    int fpos_time;
    int fcreat_time;
    int fsize;
    int fmode;
    int fpos_offset;
    char fname[MAX_FILE_NAME + 1];
    //int is_open;
};

struct super_block
{
    int16_t all_space;
    int16_t free_space;
    int16_t magic_number;
    char fs_name[12];
    struct inode_part inode_table[INODECOUNT];
};

void info()
{
    printf("General Info:\n");
    printf("Super Block: [%ld] bytes\n", SUPERSIZE);
    printf("Inode Block: [%ld] bytes\n", INODESIZE);
    struct super_block super;
    vsync(&super);
    printf("LOG\n");
    for(int i = 0; i < 3; i++)
        printf("\tfile descriptor: %d\n\tname: %s\n\tsize: %d\n************************\n", super.inode_table[i].ufi, super.inode_table[i].fname, super.inode_table[i].fsize);
}

void vsync(struct super_block *super)
{
    lseek(general_info.fdesk, 0, SEEK_SET);
    read(general_info.fdesk, super, SUPERSIZE);
}

bool module_init(const char *path)
{
    int fd = open(path, O_CREAT | O_RDWR);
    if(fd == -1)
    {
        printf("Error creating\\opening a file\n");
        return false;
    }
    general_info.fdesk = fd;
    struct super_block test;
    test.all_space = 4096;
    test.free_space = 4096 - sizeof(struct super_block);
    test.magic_number = 7777;
    strcpy(test.fs_name, "vendetta fs");
    for(int i = 0; i < INODECOUNT; i++)
        memset(test.inode_table, 0, INODESIZE * INODECOUNT);
    write(fd, &test, sizeof(struct super_block));
    printf("File System Initialized\n");
    return true;
}

int vcreat(const char *file_name)
{
    srand(time(NULL));
    if(general_info.fdesk == 0 || file_name == NULL )
        return -1;
    if(strlen(file_name) > MAX_FILE_NAME)
    {
        printf("vcreat: file name too lagre\n");
        return -1;
    }
    struct super_block super;
    if(general_info.fdesk == -1)
    {
        printf("vcreat: error opening file\n");
        return -1;
    }
    read(general_info.fdesk, &super, SUPERSIZE);
    for(int i = 0; i < INODECOUNT; i++)
    {
        if(super.inode_table[i].ufi == 0)
        {
            super.inode_table[i].ufi = rand();
            super.inode_table[i].fcreat_time = time(NULL);
            super.inode_table[i].fpos_offset = 0;
            //super.inode_table[i].is_open = FILE_CLOSE;
            super.inode_table[i].fsize = FILE_DEFAULT_SIZE;
            super.inode_table[i].fmode = FILE_NULL;
            strcpy(super.inode_table[i].fname, file_name);
            write(general_info.fdesk, &super, SUPERSIZE);
            return super.inode_table[i].ufi;
        }
    }
    printf("Not enoght space");
}

#define SIZE 4096 * 8

static int fd;

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
}

int main()
{
    fd = open("test.txt", O_RDWR | S_IRWXU | O_CREAT);
    struct stat buf;
    fstat(fd, &buf);
    printf("Size before write(): %ld\n", buf.st_size);
    char bufg[SIZE];
    write(fd, bufg, SIZE);
    fstat(fd, &buf);
    printf("Size after lseek(): %ld\n", buf.st_size);
    //getchar();
    char *hello = "Danil Malapura\n";
    dev_write(3030, strlen(hello), hello);
    char out_array[strlen(hello) + 1];
    dev_read(3030, strlen(hello), out_array);
    printf("Output: %s\n", out_array);
    close(fd);
    remove("test.txt");
    return 0;
}

/*
    int fd = open("test.txt", O_RDWR | S_IRWXU | O_CREAT);
    struct stat buf;
    fstat(fd, &buf);
    printf("Size before write(): %ld\n", buf.st_size);
    char bufg[SIZE];
    write(fd, bufg, SIZE);
    fstat(fd, &buf);
    printf("Size after lseek(): %ld\n", buf.st_size);
    close(fd);
    getchar();
    remove("test.txt");
    //close(general_info.fdesk);
*/