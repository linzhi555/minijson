CC=gcc
CFLAGS = -Wall -Wextra -O1 -g -std=c99

ALL_O=miniparser.o minilexer.o miniutils.o minimap.o miniarray.o ministr.o mininum.o minivalue.o
ALL_H=minijson.h miniutils.h minilexer.h

all:libminijson.a mjfmt

libminijson.a:$(ALL_O)
	ar rcs libminijson.a  $^
miniparser.o:miniparser.c $(ALL_H)
minilexer.o:minilexer.c $(ALL_H)
miniutils.o:miniutils.c $(ALL_H)
minimap.o:minimap.c $(ALL_H)
miniarray.o:miniarray.c $(ALL_H)
ministr.o:ministr.c $(ALL_H)
mininum.o:mininum.c $(ALL_H)
minivalue.o:minivalue.c $(ALL_H)

mjfmt: libminijson.a
	$(CC) $(CFLAGS) ./mjfmt.c -o mjfmt -lminijson -L./

clean:
	rm -f  ./*.o ./*.a ./*.generated.c ./*.generated.h ./mjfmt

