#include <iostream>
#include <napi.h>
#include <memory>
#include "lib-ruby-parser.hpp"
#include "to_v8.hpp"

namespace lib_ruby_parser_node
{
    struct NodeValueWithEnv
    {
        explicit NodeValueWithEnv(napi_value value_, Napi::Env env_) : value(value_), env(env_){};
        explicit NodeValueWithEnv(Napi::Value value_) : value(value_), env(value_.Env()){};

        Napi::Value ToValue()
        {
            if (value == nullptr)
            {
                return env.Null();
            }
            else
            {
                return Napi::Value(env, value);
            }
        }

        Napi::Env Env()
        {
            return env;
        }

    private:
        napi_value value;
        Napi::Env env;
    };

    extern "C"
    {
        lib_ruby_parser::DecoderResultBlob decode(
            void *state,
            lib_ruby_parser::StringBlob encoding_b,
            lib_ruby_parser::ByteListBlob input_b)
        {
            lib_ruby_parser::String encoding = string_from_string_blob(encoding_b);
            lib_ruby_parser::ByteList input = byte_list_from_byte_list_blob(input_b);

            NodeValueWithEnv *js_state = static_cast<NodeValueWithEnv *>(state);
            Napi::Env env = js_state->Env();
            Napi::Value decoder = js_state->ToValue();
            delete js_state;

            if (!decoder.IsFunction())
            {
                return lib_ruby_parser::decoder_result_to_blob(
                    lib_ruby_parser::DecoderResult::Err(
                        lib_ruby_parser::InputError::DecodingError(
                            lib_ruby_parser::String::Copied("`decode` argument is not a function"))));
            }
            Napi::Function decoder_f = decoder.As<Napi::Function>();

            Napi::Uint8Array input_a = Napi::Uint8Array::New(env, input.len);
            for (size_t i = 0; i < input.len; i++)
            {
                input_a[i] = input.ptr[i];
            }
            Napi::Value decoder_output = decoder_f.Call({
                Napi::String::New(env, encoding.ptr, encoding.len),
                input_a,
            });
            if (env.IsExceptionPending())
            {
                Napi::Error e = env.GetAndClearPendingException();
                return lib_ruby_parser::decoder_result_to_blob(
                    lib_ruby_parser::DecoderResult::Err(
                        lib_ruby_parser::InputError::DecodingError(
                            lib_ruby_parser::String::Copied(e.Message().c_str()))));
            }
            if (!decoder_output.IsBuffer())
            {
                return lib_ruby_parser::decoder_result_to_blob(
                    lib_ruby_parser::DecoderResult::Err(
                        lib_ruby_parser::InputError::DecodingError(
                            lib_ruby_parser::String::Copied("`decode` function must return a Buffer"))));
            }
            Napi::Uint8Array buffer = decoder_output.As<Napi::Uint8Array>();

            return lib_ruby_parser::decoder_result_to_blob(
                lib_ruby_parser::DecoderResult::Ok(
                    lib_ruby_parser::ByteList::Copied(
                        reinterpret_cast<char *>(buffer.Data()), buffer.ElementLength())));
        }
    }

    Napi::Value parse(const Napi::CallbackInfo &info)
    {
        Napi::Env env = info.Env();

        if (info.Length() != 4)
        {
            // 1 argument is `objects` that is passed implicitly by a wrapper function
            Napi::TypeError::New(env, "Expected 3 arguments").ThrowAsJavaScriptException();
            return env.Null();
        }

        if (!info[0].IsString())
        {
            Napi::TypeError::New(env, "`input` argument must be a String").ThrowAsJavaScriptException();
            return env.Null();
        }
        std::string input = info[0].As<Napi::String>().Utf8Value();

        if (!info[1].IsString())
        {
            Napi::TypeError::New(env, "`buffer_name` argument must be a String").ThrowAsJavaScriptException();
            return env.Null();
        }
        std::string buffer_name = info[1].As<Napi::String>().Utf8Value();

        if (!info[2].IsFunction())
        {
            Napi::TypeError::New(env, "`decode` argument must be a function").ThrowAsJavaScriptException();
            return env.Null();
        }
        Napi::Value decode_fn = info[2];
        NodeValueWithEnv *decoder = new NodeValueWithEnv(decode_fn);

        Napi::Object objects = info[3].As<Napi::Object>();

        return ToV8(
            lib_ruby_parser::parse(
                lib_ruby_parser::ByteList::Copied(input.c_str(), input.size()),
                lib_ruby_parser::ParserOptions(
                    /* 1. filename */
                    lib_ruby_parser::String::Copied("(eval)"),

                    /* 2. decoder */
                    lib_ruby_parser::MaybeDecoder::Some(
                        lib_ruby_parser::Decoder(
                            decode,
                            decoder)),

                    /* 3. token_rewriter */
                    lib_ruby_parser::MaybeTokenRewriter::None(),

                    /* 4. record_tokens */
                    true)),
            env, objects);
    }

    Napi::Object Init(Napi::Env env, Napi::Object exports)
    {
        exports.Set(
            "parse",
            Napi::Function::New(env, parse));

        return exports;
    }

    NODE_API_MODULE(ruby_parser, Init)

} // namespace lib_ruby_parser_node
