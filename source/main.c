/*      Developed by Bastard
 * fix:         file mapping mechanism (repair size of superblock e.t)
 * fix:         inode/data bitmap mechanizm
 * rewrite:     dev_creat function
*/ 

#include "../header/fs_funct.h"
#include "../header/fstart.h"
#include "../header/fs_interface.h"


int main()
{
    module_init("test.vfs");
    dev_creat("Tfolder", VDIR, 0);
    grap_parser();
    module_exit();
    return 0;
    // struct inode_t inode;
    // dev_read(a, INODESIZE, &inode);
    // inode.block[0] = 32768;
    // inode.block[1] = 33792;
    // inode.block[2] = 34816;
    // inode.block[3] = 35840;
    // inode.block[4] = 36864;
    // inode.block[5] = 37888;
    // inode.block[6] = 38912;
    // dev_write(a, INODESIZE, &inode);


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

