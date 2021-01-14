#include <napi.h>
#include "lib-ruby-parser.h"
#include "convert.h"
#include <iostream>
#include <variant>
#include <tuple>

using namespace lib_ruby_parser;
using namespace Napi;

namespace lib_ruby_parser_node
{

    class JsCustomDecoder : public CustomDecoder
    {
    public:
        struct DecodeError
        {
            bool has_error;
            std::string error;
        };

        Napi::FunctionReference callback;
        std::shared_ptr<DecodeError> error;

        JsCustomDecoder(const Napi::Function &callback, std::shared_ptr<DecodeError> error)
        {
            this->callback = Napi::Persistent(callback);
            this->error = error;
        }

        Env env()
        {
            return this->callback.Env();
        }

        virtual Result rewrite(std::string encoding, Bytes input)
        {
            Value raw_response = callback.Call({
                String::New(env(), encoding),
                convert(input, env()),
            });
            if (!raw_response.IsObject())
                return JsError("response must be an object");

            Object response = raw_response.As<Object>();

            Value success = response.Get("success");
            if (!success.IsBoolean())
                return JsError("'success' field must be true/false");

            if (success.ToBoolean().Value())
            {
                // success, consume 'output' field
                if (!response.Get("output").IsArray())
                    return JsError("'output' field must be an array");

                Napi::Array output = response.Get("output").As<Napi::Array>();
                std::vector<char> bytes;
                bytes.reserve(output.Length());
                for (size_t i = 0; i < output.Length(); i++)
                {
                    Value byte = output[i];
                    if (!byte.IsNumber())
                    {
                        return JsError("'output' field contains invalid byte");
                    }
                    bytes.push_back(byte.ToNumber().Int32Value());
                }
                return Result::Ok(Bytes(std::move(bytes)));
            }
            else
            {
                // error, consume 'error' field
                if (!response.Get("error").IsString())
                    return JsError("'error' field must be a string");

                String error = String::New(env(), response.Get("error").ToString().Utf8Value());
                return Result::Error(error);
            }
        }
        virtual ~JsCustomDecoder() {}

        Result JsError(std::string message)
        {
            this->error->has_error = true;
            this->error->error = "custom_rewriter: " + message;
            return Result::Error(message);
        }
    };

    Value JsThrow(Env env, std::string message)
    {
        TypeError::New(env, message).ThrowAsJavaScriptException();
        return env.Null();
    }

    using SharedDecodeError = std::shared_ptr<JsCustomDecoder::DecodeError>;
    using BuildParserOptionsSuccess = std::tuple<ParserOptions, SharedDecodeError>;
    using BuildParserOptionsResult = std::variant<BuildParserOptionsSuccess, std::string>;

    BuildParserOptionsResult BuildParserOptions(Napi::Object &object)
    {
        ParserOptions options;
        options.record_tokens = object.Get("record_tokens").ToBoolean().Value();
        options.debug = object.Get("debug").ToBoolean().Value();
        Napi::Value buffer_name = object.Get("buffer_name");
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
            return "buffer_name must be string/undefined";
        }
        SharedDecodeError decode_error = std::make_shared<JsCustomDecoder::DecodeError>();
        Napi::Value custom_decoder = object.Get("custom_decoder");
        if (custom_decoder.IsFunction())
        {
            options.custom_decoder = std::make_unique<JsCustomDecoder>(custom_decoder.As<Function>(), decode_error);
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

    Value parse(const CallbackInfo &info)
    {
        Env env = info.Env();

        if (info.Length() != 2)
            return JsThrow(env, "Wrong number of arguments (expected 2)");

        if (!info[0].IsString())
            return JsThrow(env, "The first argument must be a string");
        std::string source = info[0].As<String>().Utf8Value();

        if (!info[1].IsObject())
            return JsThrow(env, "The second argument must be an object");
        Object js_options = info[1].As<Object>();

        auto build_result = BuildParserOptions(js_options);
        if (auto error = std::get_if<std::string>(&build_result))
        {
            return JsThrow(env, *error);
        }
        auto tuple = std::get<BuildParserOptionsSuccess>(std::move(build_result));
        auto decode_error = std::get<1>(tuple);
        auto options = std::get<0>(std::move(tuple));

        auto result = ParserResult::from_source(source, std::move(options));

        if (decode_error->has_error)
        {
            TypeError::New(env, decode_error->error)
                .ThrowAsJavaScriptException();
            return env.Null();
        }
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
