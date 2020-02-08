/*      Developed by Bastard
 * fix:         file mapping mechanism (repair size of superblock e.t)
 * fix:         inode/data bitmap mechanizm
 * develop:     advanced write/read functions
*/ 

#include "../header/fs_funct.h"
#include "../header/fstart.h"
#include "../header/fs_interface.h"


int main()
{
    int i;
    module_init("test.vfs");
    int a = dev_creat("clang.txt", VFILE);
    int b = dev_creat("Alex", VDIR);
 
    char *bmpa = (char *) malloc(80);
    dev_read(DBITMAP_START_TABLE, 80, bmpa);

    char *buf = "There is bastard's file system";
    int len = strlen(buf);
    vwrite(a, buf, len);

    char *temp = (char *) malloc(1500);
    memset(temp, 0, 1500);
    vwrite(a, temp, 1500);

    vseek(a, 30, VSEEK_SET);
    char *out = (char *) malloc(500);
    vread(a, out, 500);

    for(i = 0; i < 500; i++)
        ;
    printf("Count of 0: %d\n", i);
    info();
    return 0;
    // struct inode_t inode;
    // dev_read(a, INODESIZE, &inode);
    // inode.block[0] = 32768;
    // inode.block[1] = 34816;
    // inode.block[2] = 33792;
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

