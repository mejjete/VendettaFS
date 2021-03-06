#pragma once
#include <stdint.h>
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
#include <bitmask.h>

typedef unsigned char uchar;
struct super_block;
struct inode_t;
short __vfs_block_size;

#define VFILE 	0x214c
#define VDIR  	0x225c 
#define VMAGIC	0x4d32 
#define BYTE 1
#define KBYTE BYTE * 1024
#define MBYTE KBYTE * 1024

#define SUPERSIZE sizeof(struct super_block)
#define INODESIZE sizeof(struct inode_t)
#define INODECOUNT 80
#define BLOCKSIZE _vfs_super_block.__vfs_block_size
#define META_BLOCKSIZE KBYTE * 4

#define SUPERBLOCK_START_TABLE 0
#define IBITMAP_START_TABLE KBYTE * 4
#define DBITMAP_START_TABLE KBYTE * 8
#define INODE_START_TABLE KBYTE * 12
#define DATA_START_TABLE META_BLOCKSIZE * 8

#define INDIRECT_BLOCK_POINTER 15
#define MAX_FILE_NAME 32
#define MAX_DIR_NAME 16
#define MAX_OPENED_FILE META_BLOCKSIZE * 8

//FILE DEFINED MACROS
#define FILE_DEFAULT_SIZE KBYTE
//is open
#define FILE_OPEN   0x67
#define FILE_CLOSE  0x55
//file mode
#define FILE_NULL   0x70
#define FILE_RONLY  0x80
#define FILE_WONLY  0x90   
//for vseek
#define VSEEK_SET	0x111c
#define VSEEK_END	0x113c
#define VSEEK_CUR	0x115c

#define CHECKTYPE(S)	S == VFILE ? "FILE" : "DIR "

//color setting
#define VFS_COLOR_DEF       "\x1b[1;32m"
#define VFS_DIRCOLOR        "\x1b[1;36m"
#define ANSI_COLOR_YELLOW   "\x1b[33m"
#define ANSI_COLOR_RESET    "\x1b[0m"

struct time_set
{
	time_t time;		//what time was this file last accessed
	time_t ctime;		//what time was this file created
	time_t mtime;		//what time was this file last modified
	time_t dtime;		//what time was this inode deleted
};

/* Inodes store information about files and directories */
/* such as: file ownership, access mode (read, write,	*/
/* execute permissions), and file type					*/
struct inode_t
{
	int type;			//type of file (regular file or directory)
	int16_t mode;		//can this file be read/written/executed
	int16_t uid;		//who owns this file
	int16_t gid;		//which group does this file belong to
	int16_t links_count;//how many hard longs are there to this file
	int size;			//how many bytes are in the file
	struct time_set ttime;	//struct for time information
	int blocks;			//how many blocks have been allocated to this file
	int flags;			//how should vendetta fs use this inode
	int osd1;			//an OS-dependent field
	int block[INDIRECT_BLOCK_POINTER];		//a set of disk pointer (15 total)
	int generation;		//file version (used by NTFS)
	int file_acl;		//a new permission model beyong mode bits
	int dir_act;		//called access control list  
	//user set
	char name[MAX_FILE_NAME + 1];	//file name
	int used_size;					//virtual cursor is offset relative to begining of current inode
	int id;							//inode block pointer
	int cursor;						//cursor in the file
	int parent;						//parent folder
	char temp_trash[68];			//free space for meta_info
};

struct super_block
{
    int inode_blocks;
    int free_inode_blocks;
	int data_blocks;
	int free_data_blocks;
    int16_t magic_number;
	vfs_bitmask __Inode_bitmask_table;
	vfs_bitmask __Data_bitmask_table;
	short __vfs_block_size;
    char fs_name[12];
};

// typedef struct FS
// {
// 	int file_descriptor;			//file descriptor in guest OS
// 	char *file_sys_name;			//file system name in guest OS
// 	struct super_block super;
// 	char current_path[50];
// 	struct inode_t cdir;
// 	struct inode_t *op_inode[MAX_OPENED_FILE];
// }file_system;

// file_system *mount_file_system(const char *name);
// int unmount_file_system(file_system *fs);

//file API
extern int my_open(const char *path);
extern int my_create(const char *path);
extern int my_close(int fd);
extern int my_remove(const char *path);
extern int my_rename(const char *old, const char *new);

