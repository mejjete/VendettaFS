#include "../header/fs_funct.h"
#include "../header/fstart.h"
#include "../header/fs_interface.h"

#define SIZE KBYTE * 8

int main()
{
    module_init("test.vfs");
    vcreat("clang.txt");
    vcreat("alex_petuh.txt");
    info();
    return 0;
}

/*
    fd = open("test.txt", O_RDWR | S_IRWXU | O_CREAT);
    struct stat buf;
    fstat(fd, &buf);
    printf("Size before write(): %ld\n", buf.st_size);
    char bufg[SIZE];
    write(fd, bufg, SIZE);
    fstat(fd, &buf);
    printf("Size after lseek(): %ld\n", buf.st_size);
    //getchar();
    char *hello = "Danil Malapura\n";
    dev_write(3030, strlen(hello), hello);
    char out_array[strlen(hello) + 1];
    dev_read(3030, strlen(hello), out_array);
    printf("Output: %s\n", out_array);
    close(fd);
    remove("test.txt");
*/