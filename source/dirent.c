#include "../header/fstart.h"
#include "../header/fs_funct.h"
#include "../header/fs_interface.h"

int change_dir(const char *dir_name)
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
        int lpos = strlen(current_path);
        current_path[lpos - (strlen(cdir.name) + 1)] = '\0';
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
            strcpy(current_path + strlen(current_path), "/");
            strcpy(current_path + strlen(current_path), dir_name);
            return 0;
        }
        i++;
    }
    printf("cd: no such directory: %s\n", dir_name);
    return 1;
}

int look_dir()
{
    int i = 2;
    struct inode_t buf;
    while(cdir.block[i] != 0)
    {
        dev_read(cdir.block[i], INODESIZE, &buf);
        printf("   %5d", buf.size);
        if(buf.type == VDIR)
            printf("   %-5s   %5s   %5s   " VFS_DIRCOLOR"%s"ANSI_COLOR_RESET "\n", CHECKTYPE(buf.type), "data", "time", buf.name);
        else 
            printf("   %-5s   %5s   %5s   %s\n", CHECKTYPE(buf.type), "data", "time", buf.name);
        i++;
    }
    if(i == 2)
        printf("current directory is empty\n");
    return 0;
}

int ucat(const char *file_name)
{
    int i = 2;
    struct inode_t inode;
    while(cdir.block[i] != 0)
    {
        dev_read(cdir.block[i], INODESIZE, &inode);
        if(inode.type == VDIR)
        {
            printf("undefined behaviour\n");
            return -1;
        }
        if(strcmp(file_name, inode.name) == 0)
        {
            char *tmp = (char *) malloc(inode.used_size + 1);
            vseek(inode.id, 0, VSEEK_SET);
            vread(inode.id, tmp, inode.used_size);
            vseek(inode.id, 0, VSEEK_SET);
            write(0, tmp, inode.used_size);
            write(0, "\n", sizeof(char));
            return 0;
        }
        i++;
    }
}

int uwrite(const char *file_name, char *text)
{
    int i = 2;
    struct inode_t inode;
    while(cdir.block[i] != 0)
    {
        dev_read(cdir.block[i], INODESIZE, &inode);
        if(inode.type == VDIR)
        {
            printf("undefined behaviour\n");
            return -1;
        }
        if(strcmp(file_name, inode.name) == 0)
        {
            vwrite(inode.id, text, strlen(text));
            return 0;
        }
        i++;
    }
}