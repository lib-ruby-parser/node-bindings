#ifndef LIB_RUBY_PARSER_NODE_CONVERT_STRING_H
#define LIB_RUBY_PARSER_NODE_CONVERT_STRING_H

#include <napi.h>

namespace lib_ruby_parser_node
{
    Napi::Value convert(std::string s, Napi::Env env);
}

#endif // LIB_RUBY_PARSER_NODE_CONVERT_STRING_H
