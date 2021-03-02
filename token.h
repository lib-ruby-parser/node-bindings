#ifndef LIB_RUBY_PARSER_NODE_TOKEN_H
#define LIB_RUBY_PARSER_NODE_TOKEN_H

#include <napi.h>
#include "lib-ruby-parser.h"

namespace lib_ruby_parser_node
{
    class Token : public Napi::ObjectWrap<Token>
    {
    public:
        static Napi::FunctionReference ctor;

        static void Init(Napi::Env &env, Napi::Object &exports);

        Token(const Napi::CallbackInfo &info);
    };

    Napi::Value convert(lib_ruby_parser::Token token, Napi::Env env);
}

#endif // LIB_RUBY_PARSER_NODE_TOKEN_H
