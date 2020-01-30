#pragma once
#include <stdint.h>

typedef unsigned char uchar;
#define VFILE 	0x214c
#define VDIR  	0x111c 
#define VMAGIC	0x4d32 

struct inode_t
{
	int16_t mode;		//can this file be read/writteb/executed
	int16_t uid;		//who owns this file
	int size;			//how many bytes are in the file
	int time;			//what time was this file las acccessed
	int ctime;			//what time was this file created
	int mtime;			//what time was this file last modified
	int dtime;			//what time was this inode deleted
	int16_t gid;		//which group does this file belong to
	int16_t links_count;//how many hard longs are there to this file
	int blocks;			//how many blocks have been allocated to this file
	int flags;			//how should ext2 use this inode
	int osd1;			//an OS-dependent field
	int block[15];		//a set of disk pointer (15 total)
	int generation;		//file version (used by NTFS)
	int file_acl;		//a new permission model beyong mode bits
	int dir_act;		//called access control list  
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
