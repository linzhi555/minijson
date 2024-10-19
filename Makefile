CC=gcc
CFLAGS = -Wall -Wextra -O0 -g
ALL_O=minijson.o minilexer.o miniutils.o minijmap.o

libminijson.a:$(ALL_O)
	ar rcs libminijson.a  $^

minijson.o:minijson.c
minilexer.o:minilexer.c
miniutils.o:miniutils.c
minijmap.o:minijmap.c

clean:
	rm -f  ./*.o ./*.a ./compile_commands.json

