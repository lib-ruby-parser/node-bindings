#ifndef LIB_RUBY_PARSER_NODE_CUSTOM_DECODER_H
#define LIB_RUBY_PARSER_NODE_CUSTOM_DECODER_H

#include <napi.h>
#include "lib-ruby-parser.h"
#include "bytes.h"

namespace lib_ruby_parser_node
{

    class JsCustomDecoder : public lib_ruby_parser::CustomDecoder
    {
    public:
        Napi::FunctionReference callback;
        std::shared_ptr<std::string> error;

        JsCustomDecoder(const Napi::Function &callback)
        {
            this->callback = Napi::Persistent(callback);
            this->error = std::shared_ptr<std::string>(nullptr);
        }

        Napi::Env env()
        {
            return this->callback.Env();
        }

        virtual Result rewrite(std::string encoding, lib_ruby_parser::Bytes input)
        {
            Napi::Value response = this->callback.Call({
                Napi::String::New(env(), encoding),
                convert(std::move(input), env()),
            });

            if (response.IsString())
            {
                // error
                Napi::String js_error = Napi::String::New(env(), response.ToString().Utf8Value());
                return JsError(js_error);
            }
            // success
            auto bytes_result = Bytes::FromV8(response);
            if (bytes_result.is_err())
            {
                return JsError("custom decoder output: " + bytes_result.get_err());
            }

            auto bytes = bytes_result.get();
            return Result::Ok(std::move(bytes));
        }
        virtual ~JsCustomDecoder() {}

        Result JsError(std::string message)
        {
            auto new_error = new std::string("custom_rewriter: " + message);
            this->error.reset(new_error);
            return Result::Error(message);
        }
    };

} // namespace lib_ruby_parser_node

#endif // LIB_RUBY_PARSER_NODE_CUSTOM_DECODER_H
