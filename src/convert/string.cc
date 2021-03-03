#include "string.h"

namespace lib_ruby_parser_node
{
    Napi::Value convert(std::string s, Napi::Env env)
    {
        return Napi::String::New(env, s);
    }
}
