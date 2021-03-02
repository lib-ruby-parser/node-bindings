#include "magic_comment.h"
#include "loc.h"

namespace lib_ruby_parser_node
{
    Napi::FunctionReference MagicComment::ctor;

    void MagicComment::Init(Napi::Env &env, Napi::Object &exports)
    {
        Napi::HandleScope scope(env);

        Napi::Function ctor = DefineClass(
            env,
            "MagicComment",
            {
                InstanceValue("kind", env.Null(), (napi_property_attributes)(napi_writable | napi_enumerable | napi_configurable)),
                InstanceValue("key_l", env.Null(), (napi_property_attributes)(napi_writable | napi_enumerable | napi_configurable)),
                InstanceValue("value_l", env.Null(), (napi_property_attributes)(napi_writable | napi_enumerable | napi_configurable)),
            });
        exports.Set("MagicComment", ctor);

        MagicComment::ctor = Napi::Persistent(ctor);
        MagicComment::ctor.SuppressDestruct();
    }

    MagicComment::MagicComment(const Napi::CallbackInfo &info) : Napi::ObjectWrap<MagicComment>(info)
    {
        auto env = info.Env();
        auto self = info.This().As<Napi::Object>();

        if (info.Length() == 3)
        {
            self.Set("kind", info[0]);
            self.Set("key_l", info[1]);
            self.Set("value_l", info[2]);
        }
        else
        {
            Napi::TypeError::New(env, "new MagicComment() takes 3 arguments").ThrowAsJavaScriptException();
        }
    }

    Napi::Value convert(lib_ruby_parser::MagicComment magic_comment, Napi::Env env)
    {
        Napi::String kind;
        switch (magic_comment.kind)
        {
        case lib_ruby_parser::MagicCommentKind::ENCODING:
            kind = Napi::String::New(env, "encoding");
            break;
        case lib_ruby_parser::MagicCommentKind::FROZEN_STRING_LITERAL:
            kind = Napi::String::New(env, "frozen-string-literal");
            break;
        case lib_ruby_parser::MagicCommentKind::WARN_INDENT:
            kind = Napi::String::New(env, "warn-indent");
            break;
        case lib_ruby_parser::MagicCommentKind::SHAREABLE_CONSTANT_VALUE:
            kind = Napi::String::New(env, "shareable-constant-value");
            break;
        }
        return MagicComment::ctor.New({
            kind,
            convert(std::move(magic_comment.key_l), env),
            convert(std::move(magic_comment.value_l), env),
        });
    }

    Napi::Value convert(std::vector<lib_ruby_parser::MagicComment> magic_comments, Napi::Env env)
    {
        Napi::Array arr = Napi::Array::New(env, magic_comments.size());
        for (size_t i = 0; i < magic_comments.size(); i++)
        {
            arr.Set(i, convert(std::move(magic_comments[i]), env));
        }
        return arr;
    }
}
