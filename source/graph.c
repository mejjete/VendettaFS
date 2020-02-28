#include "../header/fstart.h"
#include "../header/fs_funct.h"
#include "../header/fs_interface.h"

#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_RESET "\x1b[0m"

void prompt_help();

void graph_parser()
{
    char command[15];
    char flags[10];
    memset(flags, 0, 15);    
    printf(ANSI_COLOR_YELLOW"%s>"ANSI_COLOR_RESET " ", current_path);
    while(scanf("%s", command))
    {
        if(strcmp(command, "ls") == 0)
            look_dir(cdir.name);
        else if(strcmp(command, "cr") == 0)
        {            
            scanf("%s", flags);
            dev_creat(flags, VFILE, 0);
        }
        else if(strcmp(command, "mkdir") == 0)
        {
            scanf("%s", flags);
            dev_creat(flags, VDIR, 0);
        }
        else if(strcmp(command, "cd") == 0)
        {
            scanf("%s", flags);
            change_dir(flags);
        }
        else if(strcmp(command, "rm") == 0)
        {
            scanf("%s", flags);
            vremove(flags);
        }
        else if(strcmp(command, "cat") == 0)
        {
            scanf("%s", flags);
            ucat(flags);
        }
        else if(strcmp(command, "wr") == 0)
        {
            char *data = (char *) malloc(BLOCKSIZE);
            scanf("%s %s", flags, data);
            uwrite(flags, data);
        }
        else if(strcmp(command, "/help") == 0)
            prompt_help();
        else if(strcmp(command, "clear") == 0)
            system("clear");
        else if(strcmp(command, "exit") == 0)
            break;
        else 
            printf("no such command: %s\n", command);
        printf(ANSI_COLOR_YELLOW"%s>"ANSI_COLOR_RESET " ", current_path);
    }
}

void prompt_help()
{
    printf("clear               - clear working place\n");
    printf("ls                  - look at the folder\n");
    printf("cr    [file]        - creat a file\n");
    printf("rm    [file]        - remove file or directory\n");
    printf("cd    [name]        - change current directory\n");
    printf("cat   [file]        - look in the file\n");
    printf("wr    [file] [data] - write data into the file\n");
    printf("mkdir [name]        - creat directory\n");
    printf("exit\n");
}