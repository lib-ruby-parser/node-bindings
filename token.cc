#include "token.h"
#include "bytes.h"
#include "loc.h"

namespace lib_ruby_parser_node
{
    Napi::FunctionReference Token::ctor;

    void Token::Init(Napi::Env &env, Napi::Object &exports)
    {
        Napi::HandleScope scope(env);

        Napi::Function ctor = DefineClass(
            env, "Token",
            {
                InstanceValue("name", env.Null(), (napi_property_attributes)(napi_writable | napi_enumerable | napi_configurable)),
                InstanceValue("value", env.Null(), (napi_property_attributes)(napi_writable | napi_enumerable | napi_configurable)),
                InstanceValue("loc", env.Null(), (napi_property_attributes)(napi_writable | napi_enumerable | napi_configurable)),
            });
        exports.Set("Token", ctor);

        Token::ctor = Napi::Persistent(ctor);
        Token::ctor.SuppressDestruct();
    }

    Token::Token(const Napi::CallbackInfo &info) : Napi::ObjectWrap<Token>(info)
    {
    }

    Napi::Value convert(lib_ruby_parser::Token token, Napi::Env env)
    {
        auto result = Token::ctor.New({});

        result.Set("name", Napi::Value::From(env, token.name()));
        result.Set("value", Napi::Value::From(env, Bytes(std::move(token.token_value)).ToV8(env)));
        result.Set("loc", convert(std::move(token.loc), env));

        return result;
    }
}
