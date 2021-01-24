UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	PLATFORM = linux-x86_64
endif
ifeq ($(UNAME_S),Darwin)
	PLATFORM = darwin-x86_64
endif

VERSION = 3.0.0-3.2

ifndef BUILD_ENV
	BUILD_ENV = debug
endif

ifeq ($(BUILD_ENV), debug)
	NODE_GYP_FLAGS = -d
	GYP_ENV = Debug
else
	NODE_GYP_FLAGS =
	GYP_ENV = Release
endif

ASSET_PREFIX = https://github.com/lib-ruby-parser/cpp-bindings/releases/download/v$(VERSION)
LIB_RUBY_PARSER_H_URL = $(ASSET_PREFIX)/lib-ruby-parser.h
LIB_RUBY_PARSER_A_URL = $(ASSET_PREFIX)/$(PLATFORM).a

configure:
	node-gyp configure

generate-bindings:
	cd build-convert && cargo build

.PHONY: build
build:
	node-gyp build $(NODE_GYP_FLAGS)

GYP_OUTPUT = ./build/$(GYP_ENV)/ruby_parser.node
test:
	./test.js $(GYP_OUTPUT)

clean:
	rm -f $(GYP_OUTPUT)
	rm -f convert_gen.h

# // cpp bindings files

update-cpp-bindings:
	wget $(LIB_RUBY_PARSER_H_URL) -O lib-ruby-parser.h
	wget $(LIB_RUBY_PARSER_A_URL) -O lib-ruby-parser.a
