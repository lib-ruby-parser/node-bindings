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
	LIB_ASSET_NAME = libruby_parser_cpp-x86_64-pc-windows-msvc.lib
	LOCAL_LIB_NAME = lib-ruby-parser.lib
	NODE_FILE_NAME = win32.node
	EXE = .exe
endif
ifeq ($(DETECTED_OS), Linux)
	LIB_ASSET_NAME = libruby_parser_cpp-x86_64-unknown-linux-gnu.a
	LOCAL_LIB_NAME = lib-ruby-parser.a
	NODE_FILE_NAME = linux.node
	EXE =
endif
ifeq ($(UNAME_S), Darwin)
	LIB_ASSET_NAME = libruby_parser_cpp-x86_64-apple-darwin.a
	LOCAL_LIB_NAME = lib-ruby-parser.a
	NODE_FILE_NAME = darwin.node
	EXE =
endif

VERSION = 4.0.3+ruby-3.1.1

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
HEADER_URL = $(ASSET_PREFIX)/lib-ruby-parser.hpp
LIB_URL = $(ASSET_PREFIX)/$(LIB_ASSET_NAME)

include codegen/build.mk

setup:
	npm install --ignore-scripts
	npm install node-gyp --no-save
	cp LICENSE pkg/
	cp README.md pkg/

configure:
	node ./node_modules/node-gyp/bin/node-gyp.js configure

GYP_OUTPUT = ./build/$(GYP_ENV)/ruby_parser.node

.PHONY: build
build: do-codegen
	node ./node_modules/node-gyp/bin/node-gyp.js build $(NODE_GYP_FLAGS)
	cp $(GYP_OUTPUT) pkg/$(NODE_FILE_NAME)
	cp types.d.ts pkg/index.d.ts

test:
	npm run test

clean:
	rm -f $(GYP_OUTPUT)
	rm -f $(CLEAN)

download-cpp-bindings:
	$(call download_file, $(HEADER_URL), src/lib-ruby-parser.hpp)
	$(call download_file, $(LIB_URL), $(LOCAL_LIB_NAME))
