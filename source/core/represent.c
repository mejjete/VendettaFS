#include <fstart.h>
#include <fs_funct.h>
#include <fs_interface.h>
#include <bitmask.h>

bool module_init(const char *path, size_t cluster_size)
{
    struct stat buf;
    int offlag = O_RDWR;
    if(stat(path, &buf) == 0)
    {
        fd = open(path, O_RDWR);
        dev_read(INODE_START_TABLE, INODESIZE, &cdir);

        //init super block
        dev_read(SUPERBLOCK_START_TABLE, SUPERSIZE, &_vfs_super_block);
        _vfs_super_block.__Inode_bitmask_table.bitmask = (uint8_t *) malloc(META_BLOCKSIZE * 8);
        _vfs_super_block.__Data_bitmask_table.bitmask = (uint8_t *) malloc(META_BLOCKSIZE * 8);
        dev_read(IBITMAP_START_TABLE, META_BLOCKSIZE, _vfs_super_block.__Inode_bitmask_table.bitmask);
        dev_read(DBITMAP_START_TABLE, META_BLOCKSIZE, _vfs_super_block.__Data_bitmask_table.bitmask);

        strcpy(current_path, cdir.name);
        return true;
    }
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    fd = open(path, O_RDWR | O_CREAT, mode);

    if(fd == -1)
    {
        printf("Error creating\\opening a file\n");
        return false;
    }
    
    char *temp = (char *) malloc(META_BLOCKSIZE);
    memset(temp, 0, META_BLOCKSIZE);
    //init super_block
    write(fd, temp, SUPERSIZE);
    write(fd, temp, META_BLOCKSIZE - SUPERSIZE);
    printf("module: Super Block Initialized\n");    
    
    //init inode bitmap block
    //availaible only 32768 inode to indexing
    // vfs_bitmask Ibitmap = create_bitmask(4096);
    // memset(Ibitmap.bitmask, 0, 4096);
    write(fd, temp, META_BLOCKSIZE);
    printf("module: Inode Bitmap Initialized\n");

    //init data bitmap block
    //availaible only 32768 data block to indexing
    // vfs_bitmask Dbitmap = create_bitmask(4096);
    write(fd, temp, META_BLOCKSIZE);
    printf("module: Data Bitmap Initialized\n");

    //init inode table
    for(int i = 0; i < 5; i++)
        write(fd, temp, META_BLOCKSIZE);
    // printf("module: Inode Blocks Initialized\n");

    //init data block
    for(int i = 0; i < 80; i++)
        write(fd, temp, BLOCKSIZE);

    //meta info
    fstat(fd, &buf);
    printf("module: File Size: [%ld]\n", buf.st_size);
    printf("module: File System Initialized\n");

    // struct super_block super;
    _vfs_super_block.inode_blocks = 32768; 
    _vfs_super_block.free_inode_blocks = 32768;
    _vfs_super_block.data_blocks = 32768;
    _vfs_super_block.free_data_blocks = 32768;
    _vfs_super_block.__Inode_bitmask_table = create_bitmask(META_BLOCKSIZE * 8);
    _vfs_super_block.__Data_bitmask_table = create_bitmask(META_BLOCKSIZE * 8);
    _vfs_super_block.__vfs_block_size = cluster_size;
    _vfs_super_block.magic_number = VMAGIC;
    strcpy(_vfs_super_block.fs_name, "vendetta fs");

    //init root directory
    cdir.id = get_free_inode();
    strcpy(cdir.name, "root");
    cdir.block[0] = cdir.id;
    cdir.block[1] = cdir.id;
    cdir.type = VDIR;
    cdir.size = INODESIZE;
    strcpy(current_path, "root");
    dev_write(INODE_START_TABLE, INODESIZE, &cdir);
    free(temp);
    return true;
}

bool module_exit()
{
    dev_write(SUPERBLOCK_START_TABLE, SUPERSIZE, &_vfs_super_block);
    dev_write(IBITMAP_START_TABLE, _vfs_super_block.__Inode_bitmask_table.size, _vfs_super_block.__Inode_bitmask_table.bitmask);
    dev_write(DBITMAP_START_TABLE, _vfs_super_block.__Data_bitmask_table.size, _vfs_super_block.__Data_bitmask_table.bitmask);
    dev_write(cdir.id, INODESIZE, &cdir);
    close(fd);
    free(_vfs_super_block.__Inode_bitmask_table.bitmask);
    free(_vfs_super_block.__Data_bitmask_table.bitmask);
    return true;
}

//return file descriptor
int dev_creat(const char *file_name, int type, int reqsize)
{
    if(fd == 0 || file_name == NULL)
        return -1;
    if(strlen(file_name) > MAX_FILE_NAME)
        return -1;

    int i = 2, j;
    struct inode_t inode;
    //check if such file (file_name) already exist
    while(cdir.block[i] != 0)
    {
        dev_read(cdir.block[i], INODESIZE, &inode);
        if(strcmp(file_name, inode.name) == 0)
            return -1;
        i++;
    }

    memset(&inode, 0, INODESIZE);
    
    inode.id = get_free_inode();
    inode.type = type;
    while(cdir.block[i] != 0)
        i++;
    cdir.block[i] = inode.id;
    if(type == VDIR)
    {
        if(strchr(file_name, '.'))
            return -1;
        strcpy(inode.name, file_name);
        inode.size = INODESIZE;
        inode.block[0] = inode.id;
        inode.block[1] = cdir.id;
        dev_write(inode.id, INODESIZE, &inode);
        dev_write(cdir.id, INODESIZE, &cdir);
        return inode.id;
    }
    if(!strchr(file_name, '.'))
    {
        strcpy(inode.name, file_name);
        strcpy(inode.name + strlen(inode.name), ".tmp");
    }
    else 
        strcpy(inode.name, file_name);
    inode.block[0] = get_free_block(_vfs_super_block.__Data_bitmask_table);  
    inode.cursor = inode.block[0];
    inode.parent = cdir.id;
    inode.used_size = 0;
    inode.size = BLOCKSIZE;
    dev_write(inode.id, INODESIZE, &inode);
    dev_write(cdir.id, INODESIZE, &cdir);
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
    // printf("such file doesn't exist: %s\n", file_name);
    return -1;
}

//remove file
int vremove(const char *file_name)
{
    int i = 2, j = 0, k;
    struct inode_t inode;
    while(cdir.block[i] != 0)
    {
        dev_read(cdir.block[i], INODESIZE, &inode);
        if(strcmp(inode.name, file_name) == 0)
        {
            //if file is a directory
            if(inode.type == VDIR)
            {
                if(inode.block[2] != 0)
                    return -1;
            }
            cdir.block[i] = 0;
            dev_write(cdir.id, INODESIZE, &cdir);
            while(inode.block[j] != 0)
            {
                int index = (inode.block[j] - DATA_START_TABLE) / BLOCKSIZE;
                release_bitmask_block(index, &_vfs_super_block.__Data_bitmask_table);
                inode.block[j] = 0;
                j++;
            }

            //shift left indirect pointer
            while(cdir.block[i + 1] != 0)
            {
                cdir.block[i] = cdir.block[i + 1];
                i++;
            }
            cdir.block[i] = 0;
            dev_write(cdir.id, INODESIZE, &cdir);
            int temp = inode.id;
            inode.used_size = 0;
            inode.size = 0;
            inode.id = 0;
            inode.parent = 0;
            *inode.name = '\0';
            int index = (temp - INODE_START_TABLE) / INODESIZE;
            uint8_t *ibit = &_vfs_super_block.__Data_bitmask_table.bitmask[index % 8];
            release_bitmask_block(index, &_vfs_super_block.__Inode_bitmask_table);
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
                        inode.block[j] = get_free_block(_vfs_super_block.__Data_bitmask_table);
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
    vfs_bitmask mask = _vfs_super_block.__Data_bitmask_table;
    for(int i = 0; i < mask.size / BITSIZE; i++)
    {  
        for(int j = 0; j < BITSIZE; j++)
        {
            uint8_t *num = &mask.bitmask[i];
            if(!(*num & (1 << j)))
            {
                set_bit(num, j);
                _vfs_super_block.free_data_blocks--;
                return DATA_START_TABLE + (BLOCKSIZE * (i * 8 + j));
            }
        }
    }
}

int get_free_inode()
{
    vfs_bitmask mask = _vfs_super_block.__Inode_bitmask_table;
    for(int i = 0; i < mask.size / BITSIZE; i++)
    {  
        for(int j = 0; j < BITSIZE; j++)
        {
            uint8_t *num = &mask.bitmask[i];
            if(!(*num & (1 << j)))
            {
                set_bit(num, j);
                _vfs_super_block.free_inode_blocks--;
                return INODE_START_TABLE + (INODESIZE * (i * 8 + j));
            }
        }
    }
    // assert();
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

void set_bitmap(off_t offset, char n)
{
    dev_write(offset, sizeof(char), &n);
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
