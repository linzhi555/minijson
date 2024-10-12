ALL_O=minijson.o
CC=gcc
CFLAGS = -Wall -Wextra -O0 -g
libminijson.a:$(ALL_O)
	ar rcs libminijson.a  $^

minijson.o:./minijson.c

.PHONY:test
clean:
	rm -f ./test/test1 ./*.o ./*.a ./compile_commands.json
test:
	make clean
	make
	$(CC) $(CFLAGS) ./test/test1.c -o ./test/test1 -I./ -L./ -lminijson
	./test/test1
