#ifndef LIB_RUBY_PARSER_NODE_LOC_H
#define LIB_RUBY_PARSER_NODE_LOC_H

#include <napi.h>
#include "lib-ruby-parser.h"

namespace lib_ruby_parser_node
{
    class Loc : public Napi::ObjectWrap<Loc>
    {
    public:
        static Napi::FunctionReference ctor;

        static void Init(Napi::Env &env, Napi::Object &exports);

        Napi::Value Source(const Napi::CallbackInfo &info);

        Loc(const Napi::CallbackInfo &info);
        static Napi::Value New(const Napi::CallbackInfo &info, std::unique_ptr<lib_ruby_parser::Loc> loc);
    };

    Napi::Value convert(std::unique_ptr<lib_ruby_parser::Loc> loc, Napi::Env env);
}

#endif // LIB_RUBY_PARSER_NODE_LOC_H
