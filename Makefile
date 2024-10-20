CC=gcc
CFLAGS = -Wall -Wextra -O0 -g -std=c99
ALL_O=minijson.o minilexer.o miniutils.o minimap.o miniarray.o

libminijson.a:$(ALL_O)
	ar rcs libminijson.a  $^

minijson.o:minijson.c
minilexer.o:minilexer.c
miniutils.o:miniutils.c
minimap.o:minimap.c
miniarray.o:miniarray.c
clean:
	rm -f  ./*.o ./*.a ./compile_commands.json

