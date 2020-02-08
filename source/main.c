/*      Developed by Bastard
 * fix:         file mapping mechanism (repair size of superblock e.t)
 * fix:         inode/data bitmap mechanizm
 * fix:         mechanism of moving cursor
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

    struct inode_t inode;
    dev_read(a, INODESIZE, &inode);
    inode.block[0] = 37888;
    //inode.block[1] = 33792;
    //inode.block[2] = 34816;
    inode.block[1] = 35840;
    inode.block[2] = 36865;
    inode.block[3] = 32768;
    inode.block[4] = 35840; 
    dev_write(a, INODESIZE, &inode);

    vseek(a, 3132, VSEEK_SET);
    printf("After -1500 moving: [%d]\n", vseek(a, -1500, VSEEK_CUR));
    //printf("After 2 moving: [%d]\n", vseek(a, -2300, VSEEK_CUR));
    //info();
    exit(EXIT_FAILURE);
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

