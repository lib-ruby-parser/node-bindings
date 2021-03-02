#include "parser_result.h"
#include "node.h"
#include "token.h"
#include "diagnostic.h"
#include "comment.h"
#include "magic_comment.h"
#include "input.h"

namespace lib_ruby_parser_node
{
    Napi::FunctionReference ParserResult::ctor;

    void ParserResult::Init(Napi::Env &env, Napi::Object &exports)
    {
        Napi::HandleScope scope(env);

        Napi::Function ctor = DefineClass(
            env,
            "ParserResult",
            {
                InstanceValue("name", env.Null(), (napi_property_attributes)(napi_writable | napi_enumerable | napi_configurable)),
                InstanceValue("message", env.Null(), (napi_property_attributes)(napi_writable | napi_enumerable | napi_configurable)),
                InstanceValue("loc", env.Null(), (napi_property_attributes)(napi_writable | napi_enumerable | napi_configurable)),
            });
        exports.Set("ParserResult", ctor);

        ParserResult::ctor = Napi::Persistent(ctor);
        ParserResult::ctor.SuppressDestruct();
    }

    ParserResult::ParserResult(const Napi::CallbackInfo &info) : Napi::ObjectWrap<ParserResult>(info)
    {
        auto env = info.Env();
        auto self = info.This().As<Napi::Object>();

        if (info.Length() == 6)
        {
            self.Set("ast", info[0]);
            self.Set("tokens", info[1]);
            self.Set("diagnostics", info[2]);
            self.Set("comments", info[3]);
            self.Set("magic_comments", info[4]);
            self.Set("input", info[5]);
        }
        else
        {
            Napi::TypeError::New(env, "new ParserResult() takes 6 arguments").ThrowAsJavaScriptException();
        }
    }

    Napi::Value convert(std::unique_ptr<lib_ruby_parser::ParserResult> result, Napi::Env env)
    {
        if (!result)
        {
            return env.Null();
        }
        return ParserResult::ctor.New({
            convert(std::move(result->ast), env),
            convert(std::move(result->tokens), env),
            convert(std::move(result->diagnostics), env),
            convert(std::move(result->comments), env),
            convert(std::move(result->magic_comments), env),
            convert(std::move(result->input), env),
        });
    }
}
