CFLAGS=-std=gnu99 -g -O2 -Wall -I src

.PHONY: clean test

repl: src/libminilisp.c repl.c

clean:
	rm -f build/*

test: repl
	@./test.sh

wasm:
	emcc -O3 \
		-I src \
		-s WASM=1 \
		-s EXPORTED_FUNCTIONS='["_malloc", "_free"]' \
		-s EXTRA_EXPORTED_RUNTIME_METHODS='["cwrap"]' \
		-s ABORTING_MALLOC=0 \
		src/libminilisp.c wasm.c \
		-o build/unlisp.js
