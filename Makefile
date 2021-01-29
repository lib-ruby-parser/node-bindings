ifeq ($(OS), Windows_NT)
	DETECTED_OS = Windows
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S), Linux)
		DETECTED_OS = Linux
	else
		ifeq ($(UNAME_S), Darwin)
			DETECTED_OS = Darwin
		else
			DETECTED_OS = Unknown
		endif
	endif
endif

ifeq ($(DETECTED_OS), Windows)
	LIB_ASSET_NAME = lib-ruby-parser-x86_64-pc-windows-msvc.lib
	LOCAL_LIB_NAME = lib-ruby-parser.lib
endif
ifeq ($(DETECTED_OS), Linux)
	LIB_ASSET_NAME = lib-ruby-parser-x86_64-unknown-linux-gnu.a
	LOCAL_LIB_NAME = lib-ruby-parser.a
endif
ifeq ($(UNAME_S), Darwin)
	LIB_ASSET_NAME = lib-ruby-parser-x86_64-apple-darwin.a
	LOCAL_LIB_NAME = lib-ruby-parser.a
endif

VERSION = 3.0.0-3.4

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
HEADER_URL = $(ASSET_PREFIX)/lib-ruby-parser.h
LIB_URL = $(ASSET_PREFIX)/$(LIB_ASSET_NAME)

setup:
	npm install --ignore-scripts
	npm install node-gyp --no-save

configure:
	node ./node_modules/node-gyp/bin/node-gyp.js configure

generate-node-bindings:
	cd build-convert && cargo build

.PHONY: build
build:
	node ./node_modules/node-gyp/bin/node-gyp.js build $(NODE_GYP_FLAGS)

GYP_OUTPUT = ./build/$(GYP_ENV)/ruby_parser.node
test:
	./test.js $(GYP_OUTPUT)

clean:
	rm -f $(GYP_OUTPUT)
	rm -f convert_gen.h

# // cpp bindings files

download-cpp-bindings:
	wget -q $(HEADER_URL) -O lib-ruby-parser.h
	wget -q $(LIB_URL) -O $(LOCAL_LIB_NAME)
