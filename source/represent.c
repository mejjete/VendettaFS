#include "../header/fstart.h"
#include "../header/fs_funct.h"
#include "../header/fs_interface.h"

void info()
{
    #ifdef NDEBUG
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
    #endif
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
    // int i = 2;
    // printf("INODE TREE:\n");
    // struct inode_t root;
    // struct inode_t buf;
    // dev_read(INODE_START_TABLE, INODESIZE, &root);
    // while(root.block[i] != 0)
    // {
    //     dev_read(root.block[i], INODESIZE, &buf);
    //     printf("\t%5d   %-6s   %2s   %2s   %s\n", buf.size, CHECKTYPE(buf.type), "data", "time", buf.name);
    //     i++;
    // }
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
        return false;
    }
    struct super_block super;
    super.all_space = 54528;
    super.free_space = super.all_space;
    super.magic_number = VMAGIC;
    strcpy(super.fs_name, "vendetta fs");
    
    char *temp = (char *) malloc(META_BLOCKSIZE);
    memset(temp, 0, META_BLOCKSIZE);
    //init super_block
    write(fd, &super, SUPERSIZE);
    write(fd, temp, META_BLOCKSIZE - SUPERSIZE);
    // printf("module: Super Block Initialized\n");    
    
    //init inode bitmap block
    //availaible only 80 inode to indexing
    // size_t ibitmap = 0;
    // write(fd, &ibitmap, sizeof(size_t));
    // write(fd, temp, META_BLOCKSIZE - sizeof(size_t));
    write(fd, temp, META_BLOCKSIZE);
    // printf("module: Inode Bitmap Initialized\n");

    //init data bitmap block
    //availaible only 80 data block to indexing
    // size_t dbitmap = 0;
    // write(fd, &dbitmap, sizeof(size_t));
    // write(fd, temp, META_BLOCKSIZE - sizeof(size_t));
    write(fd, temp, META_BLOCKSIZE);
    // printf("module: Data Bitmap Initialized\n");

    //init inode table
    for(int i = 0; i < 5; i++)
        write(fd, temp, META_BLOCKSIZE);
    // printf("module: Inode Blocks Initialized\n");

    //init data block
    for(int i = 0; i < 80; i++)
        write(fd, temp, BLOCKSIZE);

    //meta info
    fstat(fd, &buf);
    // printf("module: File Size: [%ld]\n", buf.st_size);
    // printf("module: File System Initialized\n");

    //init root directory
    cdir.id = get_free_inode();
    strcpy(cdir.name, "root");
    cdir.block[0] = cdir.id;
    cdir.block[1] = 0;
    cdir.type = VDIR;
    cdir.size = INODESIZE;
    strcpy(current_path, "root");
    dev_write(INODE_START_TABLE, INODESIZE, &cdir);
    free(temp);
    return true;
}

bool module_exit()
{
    dev_write(cdir.id, INODESIZE, &cdir);
    return true;
}


int dev_creat(const char *file_name, int type, int reqsize)
{
    if(fd == 0 || file_name == NULL )
        return -1;
    if(strlen(file_name) > MAX_FILE_NAME)
    {
        printf("vcreat: file name too lagre\n");
        return -1;
    }
 
    int i, j;
    struct inode_t inode;
    memset(&inode, 0, INODESIZE);
    char *bitmap = (char *) malloc(80);

    //find free inode block
    dev_read(IBITMAP_START_TABLE, 80, bitmap);
    for(i = 0; i < INODECOUNT; i++)
    {
        if(bitmap[i] == 0)
        {
            bitmap[i] = 1;
            break;
        }
    }
    dev_write(IBITMAP_START_TABLE + i, sizeof(char), &bitmap[i]);
    
    if(type == VDIR)
    {
        i = 2;
        inode.type = VDIR;
        inode.id = (INODE_START_TABLE + (INODESIZE * i));
        inode.size = INODESIZE;
        if(strchr(file_name, '.'))
        {
            printf("vcreat: directory cannot have \".\" in the name");
            return -1;
        }
        strcpy(inode.name, file_name);
        inode.block[0] = inode.id;
        inode.block[1] = cdir.id;
        while(cdir.block[i] != 0)
            i++;
        cdir.block[i] = inode.id;
        dev_write(cdir.id, INODESIZE, &cdir);
        dev_write(inode.id, INODESIZE, &inode);
        return inode.id;
    }

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
    dev_write(DBITMAP_START_TABLE + j, sizeof(char), &bitmap[i]);

    //set inode information
    inode.id = (INODE_START_TABLE + (INODESIZE * i));
    inode.size = BLOCKSIZE;
    inode.used_size = 0;
    inode.type = type;
    inode.blocks = 1;
    strcpy(inode.name, file_name);
    inode.block[0] = get_free_block();
    inode.cursor = inode.block[0];

    dev_write(IBITMAP_START_TABLE + j, sizeof(char), &bitmap[j]);
    dev_write(INODE_START_TABLE + (INODESIZE * i), INODESIZE, &inode);

    i = 2;
    while(cdir.block[i] != 0)
        i++;
    cdir.block[i] = inode.id;
    dev_write(cdir.id, INODESIZE, &cdir);
    free(bitmap);
    return inode.id;
}

int vremove(const char *file_name)
{
    int i = 2, j = 0;
    struct inode_t inode;
    while(cdir.block[i] != 0)
    {
        dev_read(cdir.block[i], INODESIZE, &inode);
        if(strcmp(inode.name, file_name) == 0)
        {
            cdir.block[i] = 0;
            dev_write(cdir.id, INODESIZE, &cdir);
            while(inode.block[j] != 0)
            {
                int index = (inode.block[j] - DATA_START_TABLE) / BLOCKSIZE;
                dev_write(DBITMAP_START_TABLE + index, sizeof(char), 0);
                inode.block[j] = 0;
                j++;
            }
            int temp = inode.id;
            inode.used_size = 0;
            inode.size = 0;
            inode.id = 0;
            int index = (temp - INODE_START_TABLE) / BLOCKSIZE;
            dev_write(IBITMAP_START_TABLE + index, sizeof(char), 0);
            dev_write(temp, INODESIZE, &inode);
            return 0;
        }
        i++;
    }
    return -1;
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

int vseek(int fd, off_t offset, int whence)
{
    int i = 0;
    struct inode_t inode;
    dev_read(fd, INODESIZE, &inode);
    while(inode.block[i] != 0) 
        i++;
    --i;
    if(whence == VSEEK_SET)
    {
        inode.cursor = inode.block[0];
        move_cursor(&inode, offset);
    }
    else if(whence == VSEEK_END)
        move_cursor(&inode, inode.block[i] + inode.used_size);
    else if(whence == VSEEK_CUR)
        move_cursor(&inode, offset);
    dev_write(fd, INODESIZE, &inode);
    if(inode.cursor >= DATA_START_TABLE)
        return inode.cursor;
    else 
        return -1;
}

int vwrite(int fd, void *buf, int count)
{
    int i = 0, j = 0;;
    //newr - neew to write, wd - writted
    int newr = count, wd = 0;
    struct inode_t inode;
    dev_read(fd, INODESIZE, &inode);
    if(inode.type == VDIR)
    {
        printf("write: cannot write into a directory\n");
        exit(EXIT_FAILURE);
    }
    while(inode.block[i] != 0)
    {
        if(inode.cursor >= inode.block[i] && inode.cursor <= inode.block[i] + BLOCKSIZE)
        {
            int toend = (inode.block[i] + BLOCKSIZE) - inode.cursor;
            if(toend < count)
            {
                // printf("Writting WITH stepping\n");
                j = i + 1;
                dev_write(inode.cursor, toend, buf + wd);
                move_cursor(&inode, toend);
                inode.used_size += toend;
                wd += toend;
                newr -= wd;

                while(1) 
                {
                    if(inode.block[j] == 0)
                    {
                        inode.block[j] = get_free_block();
                        inode.size += BLOCKSIZE;
                        inode.blocks++;   
                    }
                    if(newr <= BLOCKSIZE)
                    {
                        dev_write(inode.block[j], newr, buf + wd);
                        
                        move_cursor(&inode, newr);
                        inode.used_size += newr;
                        // printf("Last Cursor: [%d]\n", inode.cursor);
                        dev_write(fd, INODESIZE, &inode);
                        return 0;
                    }
                    dev_write(inode.block[j], BLOCKSIZE, buf + wd);
                    move_cursor(&inode, BLOCKSIZE);
                    inode.used_size += BLOCKSIZE;
                    wd += BLOCKSIZE;
                    newr -= BLOCKSIZE;
                    // printf("writted: [%d]\tntw: [%d]\n", wd, newr);
                    j++;
                }
            }
            else 
            {
                // printf("Writting WITHOUT stepping\n");
                dev_write(inode.cursor, count, buf);
                move_cursor(&inode, count);
                inode.used_size += count;
                dev_write(fd, INODESIZE, &inode);
                return 0;
            }
        }
        i++;
    }
    return -1;
}

int vread(int fd, void *buf, int count)
{
    int i = 0, j = 0;;
    //newr - neew to write, wd - writted
    int newr = count, wd = 0;
    struct inode_t inode;
    dev_read(fd, INODESIZE, &inode);
    if(inode.type == VDIR)
    {
        printf("read: cannot read from a directory\n");
        exit(EXIT_FAILURE);
    }

    if(count == 0)
        return 0;
    
    while(inode.block[i] != 0)
    {
        if(inode.cursor >= inode.block[i] && inode.cursor <= inode.block[i] + BLOCKSIZE)
        {
            int toend = (inode.block[i] + BLOCKSIZE) - inode.cursor;
            if(toend < count)
            {
                // printf("Reading WITH stepping\n");
                j = i + 1;
                dev_read(inode.cursor, toend, buf + wd);
                move_cursor(&inode, toend);
                wd += toend;
                newr -= wd;

                while(1) 
                {
                    if(inode.block[j] == 0)
                    {
                        return 0; 
                    }
                    if(newr <= BLOCKSIZE)
                    {
                        dev_write(inode.block[j], newr, buf + wd);
                        move_cursor(&inode, newr);
                        // printf("Last Cursor: [%d]\n", inode.cursor);
                        dev_write(fd, INODESIZE, &inode);
                        return 0;
                    }
                    dev_read(inode.block[j], BLOCKSIZE, buf + wd);
                    move_cursor(&inode, BLOCKSIZE);
                    wd += BLOCKSIZE;
                    newr -= BLOCKSIZE;
                    // printf("readed: [%d]\tntw: [%d]\n", wd, newr);
                    j++;
                }
            }
            else 
            {
                // printf("Reading WITHOUT stepping\n");
                dev_read(inode.cursor, count, buf);
                move_cursor(&inode, count);
                dev_write(fd, INODESIZE, &inode);
                return 0;
            }
        }
        i++;
    }
    return -1;
}

int get_free_block()
{
    int i = 0;
    char *bmap = (char *) malloc(80);
    dev_read(DBITMAP_START_TABLE, 80, bmap);
    while(i < 80)
    {
        if(bmap[i] == 0)
        {
            bmap[i] = 1;
            break;
        }
        i++;
    }
    dev_write(DBITMAP_START_TABLE + i, sizeof(char), &bmap[i]);
    return DATA_START_TABLE + (BLOCKSIZE * i);
}

int get_free_inode()
{
    int i = 0;
    char *imap = (char *) malloc(80);
    dev_read(IBITMAP_START_TABLE, 80, imap);
    while(i < 80)
    {
        if(imap[i] == 0)
        {
            imap[i] = 1;
            break;
        }
        i++;
    }
    dev_write(IBITMAP_START_TABLE + i, sizeof(char), &imap[i]);
    return INODE_START_TABLE + (INODESIZE * i);
}

int move_cursor(struct inode_t *inode, int cdest)
{
    if(inode == NULL || cdest == 0)
        return -1;
    int i = 0, j = 0;
    // printf("move: parts = %d\n", parts);
    // printf("data[i]: %d\ndata[i+1]: %d\n", inode->block[0], inode->block[1]);
    while(inode->block[i] != 0)
    {
        if(inode->cursor >= inode->block[i] && inode->cursor <= inode->block[i] + BLOCKSIZE)
        {  
            if(cdest >= 0)
            {
                if(cdest <= BLOCKSIZE && (((inode->block[i] + BLOCKSIZE) - inode->cursor) >= cdest))
                {
                    // printf("Moving WITHOUT steping\n");
                    inode->cursor += cdest;
                    return 0;
                }
                else   
                {
                    // printf("Movind WITH steping\n");
                    j = i;
                    int temp = cdest;
                    int temp_cpy;
                    int curs = inode->cursor;
                    // printf("Cursor: [%d]\n", curs);
                    while(!(curs + temp <= inode->block[j] && curs + temp >= inode->block[j] + BLOCKSIZE))
                    {
                        temp -= (inode->block[j] + BLOCKSIZE) - curs;
                        if(temp < 0)
                            break;
                        temp_cpy = temp;
                        curs = inode->block[j + 1];
                        // printf("Range [%d] to [%d] not suitable\n", inode->block[j], inode->block[j + 1]);
                        //printf("Current temp: [%d]\n", temp);
                        j++;
                    }   
                    // printf("--------------------------------------------------------\n");
                    // printf("Estimated range of destination: from    [%d] to [%d]\n", inode->block[j], inode->block[j] + BLOCKSIZE);
                    // printf("Estimated cdest value:                  [%d]\n", temp_cpy);
                    // printf("Estimated futher cursor position:       [%d]\n", inode->block[j] + temp_cpy);
                    // printf("--------------------------------------------------------\n");
                    //getchar();
                    //cdest -= inode->block[i + 1] - inode->cursor;
                    inode->cursor = inode->block[j] + temp_cpy; 
                    return 0;
                }
            }
            else 
            {
                // printf("Negative Number!\n");
                if(-cdest <= BLOCKSIZE && (inode->cursor - inode->block[i - 1]) >= -cdest)
                {
                    // printf("Moving without stepping\n");
                    inode->cursor += cdest;
                    return 0;
                }
                else 
                {
                    // printf("Moving WITH reverse stepping\n");
                    j = i;
                    int temp;
                    int temp_cpy = cdest;
                    int curs = inode->cursor;
                    // printf("Cursor: [%d]\n", curs);
                    while(curs - inode->block[j] >= -curs)
                    {
                        // printf("Inode block[j]: [%d]\n", inode->block[j]);
                        // printf("Current temp: [%d]\n", temp_cpy);
                        temp_cpy += (curs - inode->block[j]);
                        if(temp_cpy >= 0)
                            break;
                        temp = temp_cpy;
                        curs = inode->block[j - 1] + BLOCKSIZE;
                        j--;
                    }
                    // printf("--------------------------------------------------------\n");
                    // printf("Estimated range of destination: from    [%d] to [%d]\n", inode->block[j], inode->block[j] + BLOCKSIZE);
                    // printf("Estimated cdest value:                  [%d]\n", temp);
                    // printf("Estimated futher cursor position:       [%d]\n", inode->block[j] + temp_cpy);
                    // printf("--------------------------------------------------------\n");
                    // getchar();
                    inode->cursor = inode->block[j] + temp_cpy; 
                    return 0;
                }
            }
        }
        i++;
    }
    return -1;
}

// int admove_cursor(struct inode_t *inode, int cdest)
// {
//     int *block = inode->block;
//     int i = 0, temp;
//     if(cdest > BLOCKSIZE)
//     {
//         while(block[i])
//             temp -= (block[i] + BLOCKSIZE) 
//     }
// }

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
