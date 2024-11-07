CC=gcc
CFLAGS = -Wall -Wextra -O1 -g
LIBS=-I../ -L../ -lminijson
PY3=python3

test_bin:target/test_json target/test_str target/test_map target/test_array target/test_gen

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

target/test_gen:../mjgen.py ./test_gen/main.c ./test_gen/type.h lib-make
	$(PY3) ../mjgen.py ./test_gen/type.h
	$(CC) $(CFLAGS) -I./test_gen  ./test_gen/main.c ./test_gen/type.h.generated.c -o target/test_gen $(LIBS)


lib-make:
	cd .. && make

jsons/big_gen.json:gen_big_json.py
	$(PY3) gen_big_json.py

test:jsons/big_gen.json test_bin
	./target/test_json
	$(PY3) ./check_json_same.py
	./target/test_str
	./target/test_map
	./target/test_array
	./target/test_gen




clean:
	rm -rf target
