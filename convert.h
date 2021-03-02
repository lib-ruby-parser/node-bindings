#ifndef LIB_RUBY_PARSER_NODE_CONVERT_H
#define LIB_RUBY_PARSER_NODE_CONVERT_H

#include <napi.h>
#include "lib-ruby-parser.h"
#include "convert_gen.h"
#include "bytes.h"
#include "input.h"
#include "loc.h"
#include "token.h"
#include "diagnostic.h"
#include "comment.h"

namespace lib_ruby_parser_node
{
    Napi::FunctionReference MagicCommentCtor;
    Napi::FunctionReference ParserResultCtor;

    Napi::Value MagicCommentCtorFn(const Napi::CallbackInfo &info)
    {
        Napi::Object self = info.This().As<Napi::Object>();
        Napi::Env env = info.Env();
        self.Set("kind", info[0]);
        self.Set("key_l", info[1]);
        self.Set("value_l", info[2]);
        return env.Null();
    }

    Napi::Value ParserResultCtorFn(const Napi::CallbackInfo &info)
    {
        Napi::Object self = info.This().As<Napi::Object>();
        Napi::Env env = info.Env();
        self.Set("ast", info[0]);
        self.Set("tokens", info[1]);
        self.Set("diagnostics", info[2]);
        self.Set("comments", info[3]);
        self.Set("magic_comments", info[4]);
        self.Set("input", info[5]);
        return env.Null();
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
        return MagicCommentCtor.New({
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

    Napi::Value convert(lib_ruby_parser::Input input, Napi::Env env)
    {
        return Input::New(env, std::move(input));
    }

    Napi::Value convert(std::unique_ptr<lib_ruby_parser::ParserResult> result, Napi::Env env)
    {
        if (!result)
        {
            return env.Null();
        }
        return ParserResultCtor.New({
            convert(std::move(result->ast), env),
            convert(std::move(result->tokens), env),
            convert(std::move(result->diagnostics), env),
            convert(std::move(result->comments), env),
            convert(std::move(result->magic_comments), env),
            convert(std::move(result->input), env),
        });
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

    void InitCustomTypes(Napi::Env env, Napi::Object exports)
    {
        Napi::Function fn;

        Loc::Init(env, exports);
        Token::Init(env, exports);
        Diagnostic::Init(env, exports);
        Comment::Init(env, exports);

        fn = Napi::Function::New(env, MagicCommentCtorFn, "MagicComment");
        MagicCommentCtor = Napi::Persistent(fn);
        MagicCommentCtor.SuppressDestruct();
        exports.Set("MagicComment", fn);

        fn = Napi::Function::New(env, ParserResultCtorFn, "ParserResult");
        ParserResultCtor = Napi::Persistent(fn);
        ParserResultCtor.SuppressDestruct();
        exports.Set("ParserResult", fn);
    }
} // namespace lib_ruby_parser_node

#endif // LIB_RUBY_PARSER_NODE_CONVERT_H
