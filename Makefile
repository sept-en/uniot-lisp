CFLAGS=-std=gnu99 -g -O2 -Wall -I src

.PHONY: clean test

env: 
	$(info RUN THIS COMMAND:)
	$(info )
	$(info source ./emsdk/emsdk_env.sh --build=Release)
	$(info )

repl: src/libminilisp.c repl.c

clean:
	rm -f repl
	rm -f build/*

test: repl
	@./test.sh

wasm:
	emcc -O3 \
		-I src \
		-s WASM=1 \
		-s EXPORTED_FUNCTIONS='["_malloc", "_free"]' \
		-s EXTRA_EXPORTED_RUNTIME_METHODS='["cwrap", "AsciiToString", "writeAsciiToMemory"]' \
		src/libminilisp.c wasm.c \
		-o build/unlisp.js

		cp build/unlisp.js build/unlisp-local.js
		echo 'export default Module' >> build/unlisp.js
		echo '/* eslint-disable */' | cat - build/unlisp.js > build/unlisp.js.tmp && mv build/unlisp.js.tmp build/unlisp.js

# useful options
# -s ABORTING_MALLOC=0
# -s ALLOW_MEMORY_GROWTH=1
# -s BINARYEN_MEM_MAX=2147418112
# -s ASYNCIFY
