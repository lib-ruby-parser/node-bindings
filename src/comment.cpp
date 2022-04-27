#include "comment.hpp"
#include "loc.hpp"

namespace lib_ruby_parser_node
{
    Napi::FunctionReference Comment::ctor;

    void Comment::Init(Napi::Env &env, Napi::Object &exports)
    {
        Napi::HandleScope scope(env);

        Napi::Function ctor = DefineClass(
            env,
            "Comment",
            {
                InstanceValue("kind", env.Null(), (napi_property_attributes)(napi_writable | napi_enumerable | napi_configurable)),
                InstanceValue("location", env.Null(), (napi_property_attributes)(napi_writable | napi_enumerable | napi_configurable)),
            });
        exports.Set("Comment", ctor);

        Comment::ctor = Napi::Persistent(ctor);
        Comment::ctor.SuppressDestruct();
    }

    Comment::Comment(const Napi::CallbackInfo &info) : Napi::ObjectWrap<Comment>(info)
    {
        auto env = info.Env();
        auto self = info.This().As<Napi::Object>();

        if (info.Length() == 2)
        {
            self.Set("kind", info[0]);
            self.Set("location", info[1]);
        }
        else
        {
            Napi::TypeError::New(env, "new Comment() takes 2 arguments").ThrowAsJavaScriptException();
        }
    }

    Napi::Value convert(lib_ruby_parser::Comment comment, Napi::Env env)
    {
        Napi::String kind;
        switch (comment.kind)
        {
        case lib_ruby_parser::CommentType::INLINE:
            kind = Napi::String::New(env, "inline");
            break;
        case lib_ruby_parser::CommentType::DOCUMENT:
            kind = Napi::String::New(env, "document");
            break;
        case lib_ruby_parser::CommentType::UNKNOWN:
            kind = Napi::String::New(env, "unknown");
            break;
        }
        return Comment::ctor.New({
            kind,
            convert(std::move(comment.location), env),
        });
    }

    Napi::Value convert(std::vector<lib_ruby_parser::Comment> comments, Napi::Env env)
    {
        Napi::Array arr = Napi::Array::New(env, comments.size());
        for (size_t i = 0; i < comments.size(); i++)
        {
            arr.Set(i, convert(std::move(comments[i]), env));
        }
        return arr;
    }
}
