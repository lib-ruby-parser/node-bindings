#ifndef LIB_RUBY_PARSER_NODE_BYTES_H
#define LIB_RUBY_PARSER_NODE_BYTES_H

#include <napi.h>
#include "lib-ruby-parser.hpp"
#include "result.hpp"

namespace lib_ruby_parser_node
{
    class Bytes : public lib_ruby_parser::Bytes
    {
    public:
        explicit Bytes(lib_ruby_parser::Bytes bytes);

        Napi::Value ToV8(Napi::Env env) const;

        static Result<lib_ruby_parser::Bytes> FromV8(Napi::Value array);
    };

    Napi::Value convert(lib_ruby_parser::Bytes bytes, Napi::Env env);
} // namespace lib_ruby_parser_node

#endif // LIB_RUBY_PARSER_NODE_BYTES_H
