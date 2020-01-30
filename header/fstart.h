#pragma once
#include <stdint.h>

typedef unsigned char uchar;
#define VFILE 	0x214c
#define VDIR  	0x111c 
#define VMAGIC	0x4d32 
#define SUPERSIZE sizeof(struct super_block)
#define BLOCKSIZE 1024							//size data block on disk in bytes

struct vfile
{
	int ufi;
	int creat_time;
	int l_access_time;
	int flags;
};

struct super_block
{
	//struct super_operation *sp_op;
	int16_t blocksize;
	int16_t magic_number;
	uchar name[12];
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
