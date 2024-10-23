CC=gcc
CFLAGS = -Wall -Wextra -O0 -g
LIBS= -I../ -L../ -lminijson

ALL_TEST:test_json test_str

all:test

target:
	mkdir -P target

test_json:test_json.c
	$(CC) $(CFLAGS) $^ -o ./target/$@ $(LIBS)

test_str:test_str.c
	$(CC) $(CFLAGS) $^ -o ./target/$@ $(LIBS)

.PHONY: lib-make test
lib-make:
	cd .. && make

test:lib-make target
	make $(ALL_TEST)
	valgrind ./test_json
	valgrind ./test_str
