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

    //bitmap info
    size_t ibmpa;
    size_t dbmap;
    dev_read(IBITMAP_START_TABLE, sizeof(size_t), &ibmpa);
    dev_read(DBITMAP_START_TABLE, sizeof(size_t), &dbmap);
    //printf("Inode bitmap:\n\t");
    //bitmap(ibmpa);
    //printf("Data bitmap:\n\t");
    //bitmap(dbmap);

    struct inode_t inode[2];
    dev_read(INODE_START_TABLE, INODESIZE * 2, inode);
    printf("\nLOG\n");
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
    
    char temp[KBYTE * 4];
    //init super_block
    write(fd, &test, SUPERSIZE);
    write(fd, temp, 4096 - SUPERSIZE);    
    printf("module: Super Block Initialized\n");    
    
    //init inode bitmap block
    //availaible only 64 inode to indexing
    size_t ibitmap = 0;
    write(fd, &ibitmap, sizeof(size_t));
    write(fd, temp, BLOCKSIZE - sizeof(size_t));
    printf("module: Inode Bitmap initialized\n");

    //init data bitmap block
    //availaible only 64 inode to indexing
    size_t dbitmap = 0;
    write(fd, &dbitmap, sizeof(size_t));
    write(fd, temp, BLOCKSIZE - sizeof(size_t));
    printf("module: Data Bitmap initialized\n");

    //init inode table
    struct inode_t inode;
    memset(&inode, 0, INODESIZE);
    for(int i = 0; i < 5; i++)
        write(fd, &inode, INODESIZE);
    printf("module: Inode Blocks initialized\n");


    fstat(fd, &buf);
    printf("module: File Size: [%ld]\n", buf.st_size);
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
    if(fd == -1)
    {
        printf("vcreat: error opening file\n");
        return -1;
    }

    struct inode_t curr_inode;
    size_t imap;
    int i;
    dev_read(IBITMAP_START_TABLE, sizeof(size_t), &imap);
    set_bitmap(&imap, 1);
    bitmap(imap);
    printf("\n");
    getchar();
    for(i = 0; i < sizeof(size_t) * 4; i++)
    {
        if(get_bitmap(imap, i) == false)
            break;
    }
    printf("%d index are free\n", i + 1);
    exit(EXIT_FAILURE);
    
    /*
    for(int i = 0; i < 5; i++)
    {   
        if(inode[i].id == 0)
        {
            //set bitmask in bitmap into 1
            size_t ibmap;
            set_bitmap(&ibmap, i);

            dev_write(IBITMAP_START_TABLE, sizeof(size_t), &ibmap);

            inode[i].id = (INODE_START_TABLE + (INODESIZE * i));
            inode[i].size = BLOCKSIZE;
            strcpy(inode[i].name, file_name);
            dev_write(INODE_START_TABLE + (INODESIZE * i), INODESIZE, &inode[i]);
            return 0;
        }
    }
    */
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

void bitmap(size_t n) 
{ 
    size_t i; 
    int pos;
    for (i = 1 << 31, pos = sizeof(n); i > 0; i = i / 2, pos++) 
        (n & i)? printf("1") : printf("0"); 
    printf("\n");
} 

void bin(size_t n) 
{ 
    /* step 1 */
    if (n > 1) 
        bin(n/2); 
  
    /* step 2 */
    printf("%ld", n % 2); 
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

/*true function
void set_bit(size_t *num, int pos)
{
	*num = (1 << pos) | *num;
}

void bitmap(size_t num)
{
	size_t i;
	for (i = 1 << 31; i > 0; i = i / 2)
		(num & i) ? printf("1") : printf("0");
	printf("\n");
}

*/
