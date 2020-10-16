CC=gcc
CFLAGS=-I/home/johncoffey/Home/Programming/C/VendettaFS/header
DEPS=source/core/represent.c source/dirent.c source/explorer.c source/numop/bitmask.c
DEBUG=source/core/internal_debug.c

output: source/main.c $(DEPS) $(DEBUG)
	$(CC) -g $(CFLAGS) source/main.c $(DEPS) $(DEBUG)

clean:
	rm a.out *.vfs
