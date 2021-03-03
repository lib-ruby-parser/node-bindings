#include "size_t.h"

namespace lib_ruby_parser_node
{
    Napi::Value convert(size_t n, Napi::Env env)
    {
        return Napi::Number::New(env, n);
    }
}
