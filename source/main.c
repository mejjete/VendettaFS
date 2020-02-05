/*      Developed by Bastard
 * rewrite:     file mapping mechanism (repair size of superblock e.t)
 * rewrite:     inode/data bitmap mechanizm
 * develop:     advanced write/read functions
*/ 

#include "../header/fs_funct.h"
#include "../header/fstart.h"
#include "../header/fs_interface.h"


int main()
{
    module_init("test.vfs");
    int a = dev_creat("clang.txt", VFILE);
    int b = dev_creat("gcc.txt", VDIR);


    char buf[] = "There is bastard's file system"; 
    //char *nu = (char *) malloc(KBYTE);
    
    vwrite(a, buf, strlen(buf));
    //vwrite(a, nu, KBYTE);
    info();

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

