CFLAGS=-std=gnu99 -g -O2 -Wall

.PHONY: clean test

minilisp: libminilisp.c minilisp.c

clean:
	rm -f minilisp *~
	rm -f build/*

test: minilisp
	@./test.sh

wasm:
	emcc -O3 -s WASM=1 -s EXPORTED_FUNCTIONS='["_malloc", "_free"]' -s EXTRA_EXPORTED_RUNTIME_METHODS='["cwrap"]' -s ABORTING_MALLOC=0 minilisp.c libminilisp.c -o build/unlisp.js
