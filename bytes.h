#ifndef LIB_RUBY_PARSER_NODE_BYTES_H
#define LIB_RUBY_PARSER_NODE_BYTES_H

#include <napi.h>
#include <variant>
#include "lib-ruby-parser.h"
#include "result.h"

namespace lib_ruby_parser_node
{
    class Bytes : public lib_ruby_parser::Bytes
    {
    public:
        explicit Bytes(lib_ruby_parser::Bytes bytes)
        {
            lib_ruby_parser::Bytes(bytes.into_ptr());
        }

        Napi::Value to_v8_value(Napi::Env env) const
        {
            Napi::TypedArray array = Napi::Uint8Array::New(env, this->size());
            for (size_t i = 0; i < this->size(); i++)
            {
                array.Set(i, this->at(i));
            }
            return array;
        }

        static Result<lib_ruby_parser::Bytes> FromV8(Napi::Value array)
        {
            if (!array.IsTypedArray())
            {
                return "must be a typed array";
            }

            auto typed_array = array.As<Napi::TypedArray>();
            if (typed_array.TypedArrayType() != napi_uint8_array)
            {
                return "must be a typed array";
            }

            auto unit8_array = array.As<Napi::Uint8Array>();

            auto size = unit8_array.ElementLength();
            auto ptr = (char *)malloc(size * sizeof(char));
            memcpy(ptr, unit8_array.Data(), size);
            auto bytes = lib_ruby_parser::Bytes(ptr, size);

            return std::move(bytes);
        }
    };
} // namespace lib_ruby_parser_node

#endif // LIB_RUBY_PARSER_NODE_BYTES_H
