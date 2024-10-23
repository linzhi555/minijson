CC=gcc
CFLAGS = -Wall -Wextra -O0 -g
LIBS= -I../ -L../ -lminijson

ALL_TEST:target/test_json target/test_str

.PHONY: test clean lib-make

target/test_json:test_json.c lib-make
	@mkdir -p target
	$(CC) $(CFLAGS) test_json.c -o $@ $(LIBS)

target/test_str:test_str.c lib-make
	@mkdir -p target
	$(CC) $(CFLAGS) test_str.c -o $@ $(LIBS)

lib-make:
	cd .. && make

test: $(ALL_TEST)
	valgrind ./target/test_json
	valgrind ./target/test_str

clean:
	rm -rf target
