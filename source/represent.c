#include <fstart.h>
#include <fs_funct.h>
#include <fs_interface.h>

bool module_init(const char *path)
{
    int fd;
    struct stat buf;
    int offlag = O_RDWR;
    if(stat(path, &buf) == 0)
    {
        fd = open(path, O_RDWR);
        dev_read(INODE_START_TABLE, INODESIZE, &fsys.cdir);
        strcpy(fsys.current_path, fsys.cdir.name);
        return true;
    }
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    fd = open(path, O_RDWR | O_CREAT, mode);

    if(fd == -1)
    {
        printf("Error creating\\opening a file\n");
        return false;
    }
    struct super_block super;
    super.all_space = 114688;
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
    fsys.fd = fd;
    fsys.cdir.id = get_free_inode();
    strcpy(fsys.cdir.name, "root");
    fsys.cdir.block[0] = fsys.cdir.id;
    fsys.cdir.block[1] = fsys.cdir.id;
    fsys.cdir.type = VDIR;
    fsys.cdir.size = INODESIZE;
    strcpy(fsys.current_path, "root");
    dev_write(INODE_START_TABLE, INODESIZE, &fsys.cdir);
    free(temp);
    return true;
}

bool module_exit()
{
    dev_write(fsys.cdir.id, INODESIZE, &fsys.cdir);
    close(fsys.fd);
    return true;
}

//return file descriptor
int dev_creat(const char *file_name, int type, int reqsize)
{
    int fd = fsys.fd;
    if(fd == 0 || file_name == NULL )
        return -1;
    if(strlen(file_name) > MAX_FILE_NAME)
        return -1;

    int i = 2, j;
    struct inode_t inode;
    //check if such file (file_name) already exist
    while(fsys.cdir.block[i] != 0)
    {
        dev_read(fsys.cdir.block[i], INODESIZE, &inode);
        if(strcmp(file_name, inode.name) == 0)
            return -1;
        i++;
    }

    memset(&inode, 0, INODESIZE);
    
    inode.id = get_free_inode();
    inode.type = type;
    while(fsys.cdir.block[i] != 0)
        i++;
    fsys.cdir.block[i] = inode.id;
    if(type == VDIR)
    {
        if(strchr(file_name, '.'))
            return -1;
        strcpy(inode.name, file_name);
        inode.size = INODESIZE;
        inode.block[0] = inode.id;
        inode.block[1] = fsys.cdir.id;
        dev_write(inode.id, INODESIZE, &inode);
        dev_write(fsys.cdir.id, INODESIZE, &fsys.cdir);
        return inode.id;
    }
    if(!strchr(file_name, '.'))
    {
        strcpy(inode.name, file_name);
        strcpy(inode.name + strlen(inode.name), ".tmp");
    }
    else 
        strcpy(inode.name, file_name);
    inode.block[0] = get_free_block();  
    inode.cursor = inode.block[0];
    inode.parent = fsys.cdir.id;
    inode.used_size = 0;
    inode.size = BLOCKSIZE;
    dev_write(inode.id, INODESIZE, &inode);
    dev_write(fsys.cdir.id, INODESIZE, &fsys.cdir);
    return inode.id;
}

int vopen(const char *file_name, int16_t mode)
{
    int inodepos = INODE_START_TABLE;
    struct inode_t *inode = (struct inode_t *) malloc(INODESIZE * 5);
    dev_read(inodepos, INODESIZE * 5, inode);
    for(int i = 0; i < 16; i++)
    {
        for(int j = 0; j < 5; j++)
        {
            if(strcmp(file_name, inode[j].name) == 0 && inode[j].type != VDIR)
            {
                // if((inode[j].mode & mode) != mode)
                //     return -1;
                inode[j].cursor = inode[j].block[0];
                dev_write(inode[j].id, INODESIZE, &inode[j]);
                int temp_ind = inode[j].id;
                free(inode);
                return temp_ind;
            }
        }
        inodepos += INODESIZE * 5;
        dev_read(inodepos, INODESIZE * 5, inode);
    }
    return -1;
}

//remove file
int vremove(const char *file_name)
{
    int i = 2, j = 0, k;
    struct inode_t inode;
    while(fsys.cdir.block[i] != 0)
    {
        dev_read(fsys.cdir.block[i], INODESIZE, &inode);
        if(strcmp(inode.name, file_name) == 0)
        {
            //if file is a directory
            if(inode.type == VDIR)
            {
                if(inode.block[2] != 0)
                    return -1;
            }
            fsys.cdir.block[i] = 0;
            dev_write(fsys.cdir.id, INODESIZE, &fsys.cdir);
            while(inode.block[j] != 0)
            {
                int index = (inode.block[j] - DATA_START_TABLE) / BLOCKSIZE;
                set_bitmap(DBITMAP_START_TABLE + index, 0);
                inode.block[j] = 0;
                j++;
            }

            //shift left indirect pointer
            while(fsys.cdir.block[i + 1] != 0)
            {
                fsys.cdir.block[i] = fsys.cdir.block[i + 1];
                i++;
            }
            fsys.cdir.block[i] = 0;
            dev_write(fsys.cdir.id, INODESIZE, &fsys.cdir);
            int temp = inode.id;
            inode.used_size = 0;
            inode.size = 0;
            inode.id = 0;
            *inode.name = '\0';
            int index = (temp - INODE_START_TABLE) / INODESIZE;
            set_bitmap(IBITMAP_START_TABLE + index, 0);
            dev_write(temp, INODESIZE, &inode);
            return 0;
        }
        i++;
    }
    return -1;
}

int dev_write(off_t fist_block, size_t size, void *data)
{
    int fd = fsys.fd;
    lseek(fd, fist_block, SEEK_SET);
    write(fd, data, size);
    return 0;
}

int dev_read(off_t first_block, size_t size, void *dest)
{
    int fd = fsys.fd;
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

//return position in file
int vtell(int fd)
{
    int i = 0;
    struct inode_t inode;
    dev_read(fd, INODESIZE, &inode);
    if(inode.type != VFILE)
        return -1;
    while(!(inode.cursor >= inode.block[i] && inode.cursor <= inode.block[i] + BLOCKSIZE))
        i++;
    int difference = inode.cursor - inode.block[i];
    return (i * BLOCKSIZE) + difference;
}

//return pure cursor
int dev_tell(int fd)
{
    struct inode_t inode;
    dev_read(fd, INODESIZE, &inode);
    return inode.cursor;
}

int vwrite(int fd, void *buf, int count)
{
    int i = 0, j = 0;;
    //newr - neew to write, wd - writted
    int newr = count, wd = 0;
    struct inode_t inode;
    dev_read(fd, INODESIZE, &inode);
    if(inode.type == VDIR)
        return -1;
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
                    j++;
                }
            }
            else 
            {
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
    //newr - need to write, wd - writted
    int newr = count, wd = 0;
    struct inode_t inode;
    dev_read(fd, INODESIZE, &inode);
    if(inode.type == VDIR)
        return -1;
    if(count == 0)
        return 0;
    
    while(inode.block[i] != 0)
    {
        if(inode.cursor >= inode.block[i] && inode.cursor <= inode.block[i] + BLOCKSIZE)
        {
            int toend = (inode.block[i] + BLOCKSIZE) - inode.cursor;
            if(toend < count)
            {
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
                        dev_write(fd, INODESIZE, &inode);
                        return 0;
                    }
                    dev_read(inode.block[j], BLOCKSIZE, buf + wd);
                    move_cursor(&inode, BLOCKSIZE);
                    wd += BLOCKSIZE;
                    newr -= BLOCKSIZE;
                    j++;
                }
            }
            else 
            {
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

void set_bitmap(off_t offset, char n)
{
    dev_write(offset, sizeof(char), &n);
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
