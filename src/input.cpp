#include "input.hpp"

namespace lib_ruby_parser_node
{
    Napi::Value Input::New(Napi::Env env, lib_ruby_parser::Input input)
    {
        auto ptr = input.ptr;
        input.ptr = nullptr;

        return Napi::External<void>::New(
            env,
            ptr,
            [](Napi::Env, void *input)
            {
                delete new lib_ruby_parser::Input(input);
            });
    }

    Napi::Value convert(lib_ruby_parser::Input input, Napi::Env env)
    {
        return Input::New(env, std::move(input));
    }
}
