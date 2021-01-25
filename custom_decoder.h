#ifndef LIB_RUBY_PARSER_NODE_CUSTOM_DECODER_H
#define LIB_RUBY_PARSER_NODE_CUSTOM_DECODER_H

#include <napi.h>
#include "lib-ruby-parser.h"

namespace lib_ruby_parser_node
{

    class JsCustomDecoder : public lib_ruby_parser::CustomDecoder
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

        Napi::Env env()
        {
            return this->callback.Env();
        }

        virtual Result rewrite(std::string encoding, lib_ruby_parser::Bytes input)
        {
            Napi::Value raw_response = callback.Call({
                Napi::String::New(env(), encoding),
                convert(std::move(input), env()),
            });
            if (!raw_response.IsObject())
                return JsError("response must be an object");

            Napi::Object response = raw_response.As<Napi::Object>();

            Napi::Value success = response.Get("success");
            if (!success.IsBoolean())
                return JsError("'success' field must be true/false");

            if (success.ToBoolean().Value())
            {
                // success, consume 'output' field
                if (!response.Get("output").IsArray())
                    return JsError("'output' field must be an array");

                Napi::Array output = response.Get("output").As<Napi::Array>();
                auto ptr = (char *)malloc(output.Length());
                for (size_t i = 0; i < output.Length(); i++)
                {
                    Napi::Value byte = output[i];
                    if (!byte.IsNumber())
                    {
                        return JsError("'output' field contains invalid byte");
                    }
                    ptr[i] = byte.ToNumber().Int32Value();
                }
                return Result::Ok(lib_ruby_parser::Bytes(ptr, output.Length()));
            }
            else
            {
                // error, consume 'error' field
                if (!response.Get("error").IsString())
                    return JsError("'error' field must be a string");

                Napi::String error = Napi::String::New(env(), response.Get("error").ToString().Utf8Value());
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

} // namespace lib_ruby_parser_node

#endif // LIB_RUBY_PARSER_NODE_CUSTOM_DECODER_H
