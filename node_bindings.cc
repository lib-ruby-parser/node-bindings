#include <napi.h>
#include "lib-ruby-parser.h"
#include "convert.h"
#include "custom_decoder.h"
#include "bytes.h"
#include "result.h"
#include <iostream>
#include <variant>
#include <tuple>

namespace lib_ruby_parser_node
{
    Napi::Value JsThrow(Napi::Env env, const std::string &message)
    {
        Napi::TypeError::New(env, message).ThrowAsJavaScriptException();
        return env.Null();
    }

    class ParserOptions : public lib_ruby_parser::ParserOptions
    {
    public:
        std::shared_ptr<std::string> decode_error;

        static Result<ParserOptions> FromV8(Napi::Value value)
        {
            if (!value.IsObject())
            {
                return "parser_options must be an object";
            }
            Napi::Object object = value.As<Napi::Object>();
            ParserOptions options;

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

            Napi::Value custom_decoder = object.Get("custom_decoder");
            if (custom_decoder.IsFunction())
            {
                auto decoder = std::make_unique<JsCustomDecoder>(custom_decoder.As<Napi::Function>());
                options.decode_error = decoder->error;
                options.custom_decoder = std::move(decoder);
            }
            else if (custom_decoder.IsUndefined())
            {
                // ok, default is used
            }
            else
            {
                return "custom_decoder must be function/undefined";
            }

            return std::move(options);
        }
    };

    Result<std::unique_ptr<lib_ruby_parser::ParserResult>> parse(const Napi::CallbackInfo &info)
    {
        Napi::Env env = info.Env();

        if (info.Length() != 2)
        {
            return "Wrong number of arguments (expected 2)";
        }

        UNWRAP_RESULT(bytes, Bytes::FromV8(info[0]));
        UNWRAP_RESULT(options, ParserOptions::FromV8(info[1]));

        auto decode_error = options.decode_error;

        auto result = lib_ruby_parser::ParserResult::from_source(std::move(bytes), std::move(options));

        if (decode_error)
        {
            return std::string(*(decode_error.get()));
        }
        return std::move(result);
    }

    Napi::Value js_parse(const Napi::CallbackInfo &info)
    {
        Napi::Env env = info.Env();

        auto result = parse(info);
        if (result.is_err())
        {
            return JsThrow(env, result.get_err());
        }
        return convert(result.get(), env);
    }

    Napi::Object Init(Napi::Env env, Napi::Object exports)
    {
        exports.Set(Napi::String::New(env, "parse"),
                    Napi::Function::New(env, js_parse));

        InitCustomTypes(env, exports);
        InitNodeTypes(env, exports);

        return exports;
    }

    NODE_API_MODULE(ruby_parser, Init)

} // namespace lib_ruby_parser_node
