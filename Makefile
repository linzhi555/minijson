CC=gcc
CFLAGS = -Wall -Wextra -O0 -g -std=c99

ALL_O=minijson.o minilexer.o miniutils.o minimap.o miniarray.o ministr.o mininum.o
ALL_H=minijson.h miniutils.h minilexer.h

all:libminijson.a
minijson.o:minijson.c $(ALL_H)
minilexer.o:minilexer.c $(ALL_H)
miniutils.o:miniutils.c $(ALL_H)
minimap.o:minimap.c $(ALL_H)
miniarray.o:miniarray.c $(ALL_H)
ministr.o:ministr.c $(ALL_H)
mininum.o:mininum.c $(ALL_H)


libminijson.a:$(ALL_O)
	ar rcs libminijson.a  $^

clean:
	rm -f  ./*.o ./*.a ./compile_commands.json

