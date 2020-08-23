CC=gcc
CFLAGS=-I

main.o: source/main.c source/represent.c source/dirent.c source/graph.c
	$(CC) -g source/main.c source/represent.c source/dirent.c source/graph.c
clean:
	rm a.out test.vfs
