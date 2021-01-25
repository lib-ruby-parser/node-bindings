#include <napi.h>
#include "lib-ruby-parser.h"
#include "convert.h"
#include "custom_decoder.h"
#include <iostream>
#include <variant>
#include <tuple>

namespace lib_ruby_parser_node
{
    Napi::Value JsThrow(Napi::Env env, std::string message)
    {
        Napi::TypeError::New(env, message).ThrowAsJavaScriptException();
        return env.Null();
    }

    using SharedDecodeError = std::shared_ptr<JsCustomDecoder::DecodeError>;
    using BuildParserOptionsSuccess = std::tuple<lib_ruby_parser::ParserOptions, SharedDecodeError>;
    using BuildParserOptionsResult = std::variant<BuildParserOptionsSuccess, std::string>;

    BuildParserOptionsResult BuildParserOptions(Napi::Object &object)
    {
        lib_ruby_parser::ParserOptions options;
        options.record_tokens = object.Get("record_tokens").ToBoolean().Value();
        options.debug = object.Get("debug").ToBoolean().Value();
        Napi::Value buffer_name = object.Get("buffer_name");
        if (buffer_name.IsString())
        {
            options.buffer_name = buffer_name.As<Napi::String>().Utf8Value();
        }
        else if (buffer_name.IsUndefined())
        {
            // ok, default is used
        }
        else
        {
            return "buffer_name must be string/undefined";
        }
        SharedDecodeError decode_error = std::make_shared<JsCustomDecoder::DecodeError>();
        Napi::Value custom_decoder = object.Get("custom_decoder");
        if (custom_decoder.IsFunction())
        {
            options.custom_decoder = std::make_unique<JsCustomDecoder>(custom_decoder.As<Napi::Function>(), decode_error);
        }
        else if (custom_decoder.IsUndefined())
        {
            // ok, default is used
        }
        else
        {
            return "custom_decoder must be function/undefined";
        }

        return std::make_tuple(std::move(options), decode_error);
    }

    Napi::Value parse(const Napi::CallbackInfo &info)
    {
        Napi::Env env = info.Env();

        if (info.Length() != 2)
            return JsThrow(env, "Wrong number of arguments (expected 2)");

        if (!info[0].IsString())
            return JsThrow(env, "The first argument must be a string");
        std::string source = info[0].As<Napi::String>().Utf8Value();

        if (!info[1].IsObject())
            return JsThrow(env, "The second argument must be an object");
        Napi::Object js_options = info[1].As<Napi::Object>();

        auto build_result = BuildParserOptions(js_options);
        if (auto error = std::get_if<std::string>(&build_result))
        {
            return JsThrow(env, *error);
        }
        auto tuple = std::get<BuildParserOptionsSuccess>(std::move(build_result));
        auto decode_error = std::get<1>(tuple);
        auto options = std::get<0>(std::move(tuple));

        auto result = lib_ruby_parser::ParserResult::from_source(source, std::move(options));

        if (decode_error->has_error)
        {
            Napi::TypeError::New(env, decode_error->error)
                .ThrowAsJavaScriptException();
            return env.Null();
        }
        return convert(std::move(result), env);
    }

    Napi::Object Init(Napi::Env env, Napi::Object exports)
    {
        exports.Set(Napi::String::New(env, "parse"),
                    Napi::Function::New(env, parse));

        InitCustomTypes(env, exports);
        InitNodeTypes(env, exports);

        return exports;
    }

    NODE_API_MODULE(ruby_parser, Init)

} // namespace lib_ruby_parser_node
