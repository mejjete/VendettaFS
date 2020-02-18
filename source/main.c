/*      Developed by Bastard
 * fix:         file mapping mechanism (repair size of superblock e.t)
 * fix:         inode/data bitmap mechanizm
*/ 

#include "../header/fs_funct.h"
#include "../header/fstart.h"
#include "../header/fs_interface.h"


int main()
{
    remove("test.vfs");
    module_init("test.vfs");
    int a = dev_creat("clang", VFILE, 0);
    int b = dev_creat("gcc.c", VFILE, 0);
    int c = dev_creat("photo.jpeg", VFILE, 0);
    int d = dev_creat("Programming", VDIR, 0);
    int e = dev_creat("boss.txt", VFILE, 0);
    printf("%d\n", c);
    vremove("gcc.c");
    info();
    module_exit();
    return 0;
}



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

