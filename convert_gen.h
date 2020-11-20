#ifndef LIB_RUBY_PARSER_CONVERT_GEN_H
#define LIB_RUBY_PARSER_CONVERT_GEN_H

#include <napi.h>
#include "lib-ruby-parser.h"

using namespace lib_ruby_parser;
using namespace Napi;

namespace lib_ruby_parser_node {
    Value convert(std::unique_ptr<Node> node, Env env)
    {
        // Object result = Object::New(env);
        return env.Null();
    }
}

#endif // LIB_RUBY_PARSER_CONVERT_GEN_H