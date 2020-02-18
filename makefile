CC=gcc
CFLAGS=-I

main.o: source/main.c source/represent.c source/dirent.c source/graph.c
	$(CC) source/main.c source/represent.c source/dirent.c source/graph.c
clean:
	rm a.out