#ifndef LIB_RUBY_PARSER_NODE_INPUT_H
#define LIB_RUBY_PARSER_NODE_INPUT_H

#include <napi.h>
#include "lib-ruby-parser.hpp"
#include <iostream>

namespace lib_ruby_parser_node
{
    class Input
    {
    public:
        static Napi::Value New(Napi::Env env, lib_ruby_parser::Input input);
    };

    Napi::Value convert(lib_ruby_parser::Input input, Napi::Env env);
}

#endif // LIB_RUBY_PARSER_NODE_INPUT_H
