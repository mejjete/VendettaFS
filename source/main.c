/*      Developed by Bastard
 * rewrite:     file mapping mechanism (repair size of superblock e.t)
 * rewrite:     inode/data bitmap mechanizm
 * develop:     vwrite, vread function
*/ 

#include "../header/fs_funct.h"
#include "../header/fstart.h"
#include "../header/fs_interface.h"

#define SIZE KBYTE * 8

int main()
{
    module_init("test.vfs");
    int a = dev_creat("clang.txt");
    char buf[] = "There is bastard's file system"; 
    vwrite(a, buf, strlen(buf));
    char *dp = (char *) malloc(strlen(buf));
    vread(a, dp, strlen(buf));
    printf("output: \"%s\"\n", dp);
    info();
    /*
    module_init("test.vfs");
    vcreat("clang.txt");
    vcreat("alex_petuh.txt");
    info();

    int n;
    size_t num, newNum;

    printf("Enter any number: ");
    scanf("%ld", &num);
    printf("Enter nth bit to set (0-31): ");
    scanf("%d", &n);

    newNum = (1 << n) | num;

    printf("Bit set successfully.\n\n");
    printf("Number before setting %d bit: %ld (in decimal)\n", n, num);
    to_binary(num);
    printf("\n");
    printf("Number after setting %d bit: %ld (in decimal)\n", n, newNum);
    to_binary(newNum);
    printf("\n");
    return 0;
    */
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