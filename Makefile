CC=gcc
CFLAGS = -Wall -Wextra -O0 -g
ALL_O=minijson.o minilexer.o miniutils.o minijmap.o

all:test

libminijson.a:$(ALL_O)
	ar rcs libminijson.a  $^


minijson.o:minijson.c
minilexer.o:minilexer.c
miniutils.o:miniutils.c
minijmap.o:minijmap.c

.PHONY:test
clean:
	rm -f ./test/test1 ./*.o ./*.a ./compile_commands.json
test:libminijson.a
	$(CC) $(CFLAGS) ./test/test1.c -o ./test/test1 -I./ -L./ -lminijson
	cd ./test/ && ./test1
