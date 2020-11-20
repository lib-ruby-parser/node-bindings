test-all: test-debug test-release

test-debug: build-debug
	./test.js ./build/Debug/ruby_parser.node

test-release: build-release
	./test.js ./build/Release/ruby_parser.node

build-debug: node-gyp-configure
	node-gyp build -d

build-release: node-gyp-configure
	node-gyp build

node-gyp-configure:
	node-gyp configure

clean:
	node-gyp clean
