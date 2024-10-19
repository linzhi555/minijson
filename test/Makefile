CC=gcc
CFLAGS = -Wall -Wextra -O0 -g

test:
	cd .. && make
	$(CC) $(CFLAGS) test1.c -o test1 -I../ -L../ -lminijson
