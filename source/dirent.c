#include "../header/fstart.h"
#include "../header/fs_funct.h"
#include "../header/fs_interface.h"

int _change_dir(const char *dir_name)
{
    int i = 2;
    struct inode_t inode;

    if(strcmp(dir_name, ".") == 0)
        return 0;
    if(strcmp(dir_name, "..") == 0)
    {
        if(cdir.block[1] == 0)
            return 0;
        dev_read(cdir.block[1], INODESIZE, &inode);
        memcpy(&cdir, &inode, INODESIZE);
        return 0;
    }
    while(cdir.block[i] != 0)
    {
        dev_read(cdir.block[i], INODESIZE, &inode);
        if(strcmp(dir_name, inode.name) == 0 && inode.type == VDIR)
        {
            cdir.block[1] = cdir.id;
            dev_write(cdir.id, INODESIZE, &cdir);
            memcpy(&cdir, &inode, INODESIZE);
            return 0;
        }
        i++;
    }
    printf("cd: no such directory: %s\n", dir_name);
    return 1;
}

int _look_dir()
{
    int i = 2;
    struct inode_t buf;
    while(cdir.block[i] != 0)
    {
        dev_read(cdir.block[i], INODESIZE, &buf);
        printf("   %5d   %-5s   %2s   %2s   %s\n", buf.size, CHECKTYPE(buf.type), "data", "time", buf.name);
        i++;
    }
    if(i == 2)
        printf("current directory is empty\n");
    return 0;
}