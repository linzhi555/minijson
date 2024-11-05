CC=gcc
CFLAGS = -Wall -Wextra -O1 -g  -pg
LIBS= -I../ -L../ -lminijson

test_bin:target/test_json target/test_str target/test_map target/test_array

.PHONY: test clean lib-make

target/test_json:test_json.c lib-make
	@mkdir -p target
	$(CC) $(CFLAGS) test_json.c -o $@ $(LIBS)

target/test_str:test_str.c lib-make
	@mkdir -p target
	$(CC) $(CFLAGS) test_str.c -o $@ $(LIBS)

target/test_map:test_map.c lib-make
	@mkdir -p target
	$(CC) $(CFLAGS) test_map.c -o $@ $(LIBS)

target/test_array:test_array.c lib-make
	@mkdir -p target
	$(CC) $(CFLAGS) test_array.c -o $@ $(LIBS)



lib-make:
	cd .. && make

jsons/big_gen.json:gen_big_json.py
	python3 gen_big_json.py

test:jsons/big_gen.json test_bin
	./target/test_json
	python3 ./check_json_same.py
	./target/test_str
	./target/test_map
	./target/test_array

test-gen:
	$(CC) $(CFLAGS) -I./test_gen  ./test_gen/main.c ./test_gen/test.generated.c -o target/test_gen $(LIBS)



clean:
	rm -rf target
