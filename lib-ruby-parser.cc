#include <napi.h>
#include "lib-ruby-parser.h"
#include "convert.h"
#include <iostream>

using namespace lib_ruby_parser;
using namespace Napi;

namespace lib_ruby_parser_node
{
    Value parse(const CallbackInfo &info)
    {
        Env env = info.Env();

        if (info.Length() != 1)
        {
            TypeError::New(env, "Wrong number of arguments")
                .ThrowAsJavaScriptException();
            return env.Null();
        }

        Value arg = info[0];
        if (!arg.IsString())
        {
            TypeError::New(env, "Argument must be a string")
                .ThrowAsJavaScriptException();
            return env.Null();
        }

        std::string source = arg.As<String>().Utf8Value();
        auto result = ParserResult::from_source(source);
        return convert(std::move(result), env);
    }

    Object Init(Env env, Object exports)
    {
        exports.Set(String::New(env, "parse"),
                    Function::New(env, parse));

        InitCustomTypes(env, exports);
        InitNodeTypes(env, exports);

        return exports;
    }

    NODE_API_MODULE(ruby_parser, Init)

} // namespace lib_ruby_parser_node
