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

build-deps: build-convert build-cpp-bindings-shared

build-convert:
	cd build-convert && cargo run

build-cpp-bindings-shared:
	cd lib-ruby-parser-cpp-bindings && make cargo-build-release

clean:
	node-gyp clean
