#ifndef LIB_RUBY_PARSER_NODE_DIAGNOSTIC_H
#define LIB_RUBY_PARSER_NODE_DIAGNOSTIC_H

#include <napi.h>
#include "lib-ruby-parser.hpp"

namespace lib_ruby_parser_node
{
    class Diagnostic : public Napi::ObjectWrap<Diagnostic>
    {
    public:
        static Napi::FunctionReference ctor;

        static void Init(Napi::Env &env, Napi::Object &exports);

        Diagnostic(const Napi::CallbackInfo &info);
    };

    Napi::Value convert(lib_ruby_parser::Diagnostic diagnostic, Napi::Env env);
    Napi::Value convert(std::vector<lib_ruby_parser::Diagnostic> diagnostics, Napi::Env env);
}

#endif // LIB_RUBY_PARSER_NODE_DIAGNOSTIC_H
