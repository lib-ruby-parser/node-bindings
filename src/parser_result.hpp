#ifndef LIB_RUBY_PARSER_NODE_PARSER_RESULT_H
#define LIB_RUBY_PARSER_NODE_PARSER_RESULT_H

#include <napi.h>
#include "lib-ruby-parser.hpp"

namespace lib_ruby_parser_node
{
    class ParserResult : public Napi::ObjectWrap<ParserResult>
    {
    public:
        static Napi::FunctionReference ctor;

        static void Init(Napi::Env &env, Napi::Object &exports);

        ParserResult(const Napi::CallbackInfo &info);
    };

    Napi::Value convert(std::unique_ptr<lib_ruby_parser::ParserResult> result, Napi::Env env);
}

#endif // LIB_RUBY_PARSER_NODE_PARSER_RESULT_H
