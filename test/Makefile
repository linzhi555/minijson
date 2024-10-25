CC=gcc
CFLAGS = -Wall -Wextra -O0 -g
LIBS= -I../ -L../ -lminijson

test_bin:target/test_json target/test_str

.PHONY: test clean lib-make

target/test_json:test_json.c lib-make
	@mkdir -p target
	$(CC) $(CFLAGS) test_json.c -o $@ $(LIBS)

target/test_str:test_str.c lib-make
	@mkdir -p target
	$(CC) $(CFLAGS) test_str.c -o $@ $(LIBS)

lib-make:
	cd .. && make

jsons/big_gen.json:gen_big_json.py
	python3 gen_big_json.py

test:jsons/big_gen.json test_bin
	valgrind ./target/test_json
	python3 ./check_json_same.py
	valgrind ./target/test_str

clean:
	rm -rf target
