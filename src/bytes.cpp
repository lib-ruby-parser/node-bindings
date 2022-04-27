#include "bytes.hpp"

namespace lib_ruby_parser_node
{
    Bytes::Bytes(lib_ruby_parser::Bytes bytes) : lib_ruby_parser::Bytes(bytes.into_ptr())
    {
    }

    Napi::Value Bytes::ToV8(Napi::Env env) const
    {
        Napi::Uint8Array array = Napi::Uint8Array::New(env, this->size());
        for (size_t i = 0; i < this->size(); i++)
        {
            array[i] = this->at(i);
        }
        return array;
    }

    Result<lib_ruby_parser::Bytes> Bytes::FromV8(Napi::Value array)
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

    Napi::Value convert(lib_ruby_parser::Bytes bytes, Napi::Env env)
    {
        Napi::TypedArray array = Napi::Uint8Array::New(env, bytes.size());
        for (size_t i = 0; i < bytes.size(); i++)
        {
            array.Set(i, bytes.at(i));
        }
        return array;
    }
} // namespace lib_ruby_parser_node
