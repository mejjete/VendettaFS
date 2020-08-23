/*      Developed by Bastard
 * fix:         file mapping mechanism (repair size of superblock e.t)
 * fix:         inode/data bitmap mechanizm
 * fix:         get_free_inode and get_free_block with set_bitmap
 * 
 * test:        vseek function
 * 
 * dev:         dev_tell for user-like output
*/ 

#include "../header/fs_funct.h"
#include "../header/fstart.h"
#include "../header/fs_interface.h"

void debug_info(struct inode_t *inode)
{
	printf("---------------------------------------\n");
	printf("Adress: %d\n", inode->id);
	printf("Cursor: %d\n", inode->cursor);
	printf("Used size: %d\n", inode->used_size);
	printf("General size: %d\n", inode->size);
}

char *string = "TEST_STRING";

int flex_open(const char *string)
{
    int desc = vopen(string, 0);
    if(desc == -1)
        desc = dev_creat(string, VFILE, 0);
    return desc;
}

int file_remove(int descriptor)
{
    char c = 0;
    int index = (descriptor - INODE_START_TABLE) / INODESIZE;
    set_bitmap(IBITMAP_START_TABLE + index, 0);
}

//print first 20 blocks of inode and data bitmap
void show_inode_bitmap()
{
    printf("---INODE BITMAP--- ---DATA BITMAP---\n");
    char ibitmap[80], dbitmap[80];
    dev_read(IBITMAP_START_TABLE, 80, &ibitmap);
    dev_read(DBITMAP_START_TABLE, 80, &dbitmap);
    for(int c = 0; c < 4; c++)
    {
        for(int i = 0; i < 5; i++)
            printf("%3d", ibitmap[(c * 5) + i]);
        printf("  |");
        for(int j = 0; j < 5; j++)
            printf("%3d", dbitmap[(c * 5) + j]);
        printf("  |\n");
    }
    printf("------------------ -----------------\n");
}

int main()
{
    module_init("test.vfs");
    // int alex_text = vopen("alex.txt", 0);
    int alex_text = flex_open("alex.txt");
    // vseek(alex_text, 0, VSEEK_SET);
    // vwrite(alex_text, string, strlen(string));
    // vseek(alex_text, 0, VSEEK_SET);
    // char data[strlen(string) + 1];
    // vread(alex_text, &data, strlen(string) + 1);
    // printf("%s\n", data);
    // struct inode_t inode;
    // dev_read(alex_text, INODESIZE, &inode);
    graph_parser();
    // debug_info(&inode);
    // info();
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

