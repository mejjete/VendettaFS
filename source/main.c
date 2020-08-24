/*      Developed by Bastard
 * fix:         file mapping mechanism (repair size of superblock e.t)
 * fix:         inode/data bitmap mechanizm
 * fix:         get_free_inode and get_free_block with set_bitmap
 * fix          
 * 
 * test:        vseek function
 * 
 * dev:         dev_tell for user-like output
*/ 

#include <fstart.h>
#include <fs_funct.h>
#include <fs_interface.h>
#include <internal_debug.h>

char *string = "TEST_STRING";

int flex_open(const char *string)
{
    int desc = vopen(string, 0);
    if(desc == -1)
        desc = dev_creat(string, VFILE, 0);
    return desc;
}

int main()
{
    module_init("test.vfs");
    explorer();
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

