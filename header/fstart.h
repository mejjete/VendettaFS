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

typedef unsigned char uchar;
struct super_block;
struct inode_t;

#define VFILE 	0x214c
#define VDIR  	0x111c 
#define VMAGIC	0x4d32 
#define BYTE 1
#define KBYTE BYTE * 1024
#define MBYTE KBYTE * 1024

#define SUPERSIZE sizeof(struct super_block)
#define INODESIZE sizeof(struct inode_t)
#define INODECOUNT 80
#define BLOCKSIZE KBYTE * 4

#define SUPERBLOCK_START_TABLE 0
#define IBITMAP_START_TABLE KBYTE * 4
#define DBITMAP_START_TABLE KBYTE * 8
#define INODE_START_TABLE KBYTE * 12

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

struct inode_t
{
	int16_t mode;		//can this file be read/written/executed
	int16_t uid;		//who owns this file
	int size;			//how many bytes are in the file
	int time;			//what time was this file las acccessed
	int ctime;			//what time was this file created
	int mtime;			//what time was this file last modified
	int dtime;			//what time was this inode deleted
	int16_t gid;		//which group does this file belong to
	int16_t links_count;//how many hard longs are there to this file
	int blocks;			//how many blocks have been allocated to this file
	int flags;			//how should vendetta fs use this inode
	int osd1;			//an OS-dependent field
	int block[15];		//a set of disk pointer (15 total)
	int generation;		//file version (used by NTFS)
	int file_acl;		//a new permission model beyong mode bits
	int dir_act;		//called access control list  
	//user set
	char name[MAX_FILE_NAME + 1];	//file name
	int cursor;						//virtual cursor is offset relative to begining of current inode
	int id;
	char temp_trash[100];			//free space for meta_info
};

struct super_block
{
    int16_t all_space;
    int16_t free_space;
    int16_t magic_number;
    char fs_name[12];
};


/* Inodes store information about files and directories */
/* such as file ownership, access mode (read, write,	*/
/* execute permissions), and file type					*/

/*
struct inode_block
{
	struct inode_operation *def_op;
	struct vfile meta_file;
	int (*creat)(struct inode*, struct dentry*, int*);
};

struct super_operations
{

};

struct inode_operation
{

};

struct permission
{
	bool read;
	bool write;
};

struct group_desc
{
	
}

*/
