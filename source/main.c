#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
//#include <io.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
//#include "../header/fstart.h"

#define BYTE 1
#define KBYTE BYTE * 1024
#define MBYTE KBYTE * 1024

#define SUPERSIZE sizeof(struct super_block)
#define INODESIZE sizeof(struct inode_part)

#define MAX_FILE_NAME 32
#define MAX_DIRECTORY_NAME 16 

struct inode_part
{
    int ufi;
    int creat_time;
    int pos_offset;
    char name[32];
};

struct super_block
{
    int16_t all_space;
    int16_t free_space;
    int16_t magic_number;
    u_char name[12];
    struct inode_part data_i[5];
};

void info(int fdesc)
{
    struct super_block super;
    lseek(fdesc, 0, SEEK_SET);
    read(fdesc, &super, SUPERSIZE);
    for(int i = 0; i < 5; i++)
        printf("\tfile descriptor: %d\n\tcreat_time: %d\n\tpos_offset: %d\n", super.data_i[i].ufi, super.data_i[i].creat_time, super.data_i[i].pos_offset);
}

bool module_init(const char *path, int *fd)
{
    *fd = _open(path, O_CREAT, _S_IREAD | _S_IWRITE);
    if(*fd == -1)
    {
        printf("Error creating\\opening a file\n");
        return false;
    }
    struct super_block test;
    test.all_space = 4096;
    test.free_space = 4096 - sizeof(struct super_block);
    test.magic_number = 7777;
    strcpy(test.name, "vendetta fs");
    for(int i = 0; i < 5; i++)
        memset(test.data_i, 0, INODESIZE * 5);
    write(*fd, &test, sizeof(struct super_block));
    printf("File System Initialized\n");
    return true;
}

int vcreat(const char *fs, const char *file_name)
{
    srand(time(NULL));
    if(fs == NULL || file_name == NULL)
        return -1;
    struct super_block super;
    int fd = _open(fs, _S_IREAD);
    if(fd == -1)
    {
        printf("vcreat: error opening file\n");
        return -1;
    }
    read(fd, &super, SUPERSIZE);
    for(int i = 0; i < 5; i++)
    {
        if(super.data_i[i].ufi == 0)
        {
            super.data_i[i].ufi = rand();
            super.data_i[i].creat_time = time(NULL);
            super.data_i[i].pos_offset = 0;
            strcpy(super.data_i[i].name, file_name);
            write(fd, &super, SUPERSIZE);
            return super.data_i[i].ufi;
        }
    }
    printf("Not enoght space");
}

int main()
{
    struct stat buf;
    int fd = _open("test.vfs", _S_IREAD);
    fstat(fd, &buf);
    printf("%d", buf.st_uid);
    return 0;
}