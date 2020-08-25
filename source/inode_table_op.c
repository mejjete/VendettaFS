#include <fstart.h>
#include <fs_funct.h>
#include <fs_interface.h>

struct inode_t *find_inode(int fd)
{
    for(int i = 0; i < MAX_OPENED_FILE; i++)
        if(fsys.inode_table[i]->id == fd)
            return fsys.inode_table[i];
    return NULL;
}

void add_to_inode_table(struct inode_t *inode)
{
    for(int i = 0; i < MAX_OPENED_FILE; i++)
    {
        if(fsys.inode_table[i] == NULL)
        {
            fsys.inode_table[i] = (struct inode_t *) malloc(INODESIZE);
            memcpy(fsys.inode_table[i], inode, INODESIZE);
            return;
        }
    }
}

void synchonize_inode_table()
{
    dev_write(fsys.cdir.id, INODESIZE, &fsys.cdir);
    printf("file %s closed\n", fsys.cdir.name);
    for(int i = 0; i < MAX_OPENED_FILE; i++)
    {
        if(fsys.inode_table[i] != NULL)
        {
            dev_write(fsys.inode_table[i]->id, INODESIZE, &fsys.inode_table[i]);
            printf("file %s closed\n", fsys.inode_table[i]->name);
            free(fsys.inode_table[i]);
            fsys.inode_table[i] = NULL;
        }
    }
}