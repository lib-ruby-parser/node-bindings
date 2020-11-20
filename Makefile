test-all: test-debug test-release

test-debug: build-debug
	./test.js ./build/Debug/ruby_parser.node

test-release: build-release
	./test.js ./build/Release/ruby_parser.node

build-debug: build-deps configure
	node-gyp build -d

build-release: build-deps configure
	node-gyp build

configure:
	node-gyp configure

build-deps: run-build-convert build-cpp-bindings-shared

run-build-convert:
	cd build-convert && cargo build

build-cpp-bindings-shared:
	cd lib-ruby-parser-cpp-bindings && make cargo-build-release

clean:
	rm -rf build
	rm -rf node_modules
	rm -f convert_gen.h
	rm -rf lib-ruby-parser-cpp-bindings/target
	cd build-convert && cargo clean
