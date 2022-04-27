#include "token.hpp"
#include "bytes.hpp"
#include "loc.hpp"

namespace lib_ruby_parser_node
{
    Napi::FunctionReference Token::ctor;

    void Token::Init(Napi::Env &env, Napi::Object &exports)
    {
        Napi::HandleScope scope(env);

        Napi::Function ctor = DefineClass(
            env,
            "Token",
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
        auto env = info.Env();
        auto self = info.This().As<Napi::Object>();

        if (info.Length() == 3)
        {
            self.Set("name", info[0]);
            self.Set("value", info[1]);
            self.Set("loc", info[2]);
        }
        else
        {
            Napi::TypeError::New(env, "new Token() takes 3 arguments").ThrowAsJavaScriptException();
        }
    }

    Napi::Value convert(lib_ruby_parser::Token token, Napi::Env env)
    {
        return Token::ctor.New({
            Napi::Value::From(env, token.name()),
            Napi::Value::From(env, Bytes(std::move(token.token_value)).ToV8(env)),
            convert(std::move(token.loc), env),
        });
    }

    Napi::Value convert(std::vector<lib_ruby_parser::Token> tokens, Napi::Env env)
    {
        Napi::Array arr = Napi::Array::New(env, tokens.size());
        for (size_t i = 0; i < tokens.size(); i++)
        {
            arr.Set(i, convert(std::move(tokens[i]), env));
        }
        return arr;
    }
}
