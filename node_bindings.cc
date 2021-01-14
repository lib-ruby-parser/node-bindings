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

        if (info.Length() != 2)
        {
            TypeError::New(env, "Wrong number of arguments (expected 2)")
                .ThrowAsJavaScriptException();
            return env.Null();
        }

        Value source_arg = info[0];
        if (!source_arg.IsString())
        {
            TypeError::New(env, "The first argument must be a string")
                .ThrowAsJavaScriptException();
            return env.Null();
        }
        std::string source = source_arg.As<String>().Utf8Value();

        Value options_arg = info[1];
        if (!options_arg.IsObject())
        {
            TypeError::New(env, "The second argument must be an object")
                .ThrowAsJavaScriptException();
            return env.Null();
        }
        Object options_obj = options_arg.As<Object>();

        ParserOptions options;
        options.record_tokens = options_obj.Get("record_tokens").ToBoolean().Value();
        options.debug = options_obj.Get("debug").ToBoolean().Value();
        auto buffer_name = options_obj.Get("buffer_name");
        if (buffer_name.IsString())
        {
            options.buffer_name = buffer_name.As<String>().Utf8Value();
        }
        else if (buffer_name.IsUndefined())
        {
            // ok, default is used
        }
        else
        {
            TypeError::New(env, "buffer_name must be string/undefined")
                .ThrowAsJavaScriptException();
            return env.Null();
        }

        auto result = ParserResult::from_source(source, std::move(options));
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
