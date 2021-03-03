#ifndef LIB_RUBY_PARSER_NODE_CONVERT_SIZE_T_H
#define LIB_RUBY_PARSER_NODE_CONVERT_SIZE_T_H

#include <napi.h>

namespace lib_ruby_parser_node
{
    Napi::Value convert(size_t n, Napi::Env env);
}

#endif // LIB_RUBY_PARSER_NODE_CONVERT_SIZE_T_H
