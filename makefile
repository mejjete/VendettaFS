CC=gcc
CFLAGS=-I/home/bastard/Home/VendettaFS/header
DEPS=source/represent.c source/dirent.c source/explorer.c
DEBUG=source/internal_debug.c

output: source/main.c $(DEPS) $(DEBUG)
	$(CC) -g $(CFLAGS) source/main.c $(DEPS) $(DEBUG)

clean:
	rm a.out *.vfs
