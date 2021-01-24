#ifndef LIB_RUBY_PARSER_CONVERT_H
#define LIB_RUBY_PARSER_CONVERT_H

#include <napi.h>
#include "lib-ruby-parser.h"
#include "convert_gen.h"

namespace lib_ruby_parser_node
{
    Napi::FunctionReference LocCtor;
    Napi::FunctionReference RangeCtor;
    Napi::FunctionReference TokenCtor;
    Napi::FunctionReference DiagnosticCtor;
    Napi::FunctionReference CommentCtor;
    Napi::FunctionReference MagicCommentCtor;
    Napi::FunctionReference ParserResultCtor;

    Napi::Value LocCtorFn(const Napi::CallbackInfo &info)
    {
        Napi::Object self = info.This().As<Napi::Object>();
        Napi::Env env = info.Env();
        self.Set("begin", info[0]);
        self.Set("end", info[1]);
        return env.Null();
    }

    Napi::Value RangeCtorFn(const Napi::CallbackInfo &info)
    {
        Napi::Object self = info.This().As<Napi::Object>();
        Napi::Env env = info.Env();
        self.Set("begin_pos", info[0]);
        self.Set("end_pos", info[1]);
        return env.Null();
    }

    Napi::Value TokenCtorFn(const Napi::CallbackInfo &info)
    {
        Napi::Object self = info.This().As<Napi::Object>();
        Napi::Env env = info.Env();
        self.Set("name", info[0]);
        self.Set("value", info[1]);
        self.Set("loc", info[2]);
        return env.Null();
    }

    Napi::Value DiagnosticCtorFn(const Napi::CallbackInfo &info)
    {
        Napi::Object self = info.This().As<Napi::Object>();
        Napi::Env env = info.Env();
        self.Set("level", info[0]);
        self.Set("message", info[1]);
        self.Set("range", info[2]);
        return env.Null();
    }

    Napi::Value CommentCtorFn(const Napi::CallbackInfo &info)
    {
        Napi::Object self = info.This().As<Napi::Object>();
        Napi::Env env = info.Env();
        self.Set("kind", info[0]);
        self.Set("location", info[1]);
        return env.Null();
    }

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

    Napi::Value convert(std::unique_ptr<lib_ruby_parser::Loc> loc, Napi::Env env)
    {
        if (!loc)
        {
            return env.Null();
        }
        return LocCtor.New({Napi::Value::From(env, loc->begin),
                            Napi::Value::From(env, loc->end)});
    }

    Napi::Value convert(std::unique_ptr<lib_ruby_parser::Range> range, Napi::Env env)
    {
        if (!range)
        {
            return env.Null();
        }
        return RangeCtor.New({
            Napi::Value::From(env, range->begin_pos),
            Napi::Value::From(env, range->end_pos),
        });
    }

    Napi::Value convert(lib_ruby_parser::Token token, Napi::Env env)
    {
        return TokenCtor.New({
            Napi::Value::From(env, token.name()),
            Napi::Value::From(env, token.token_value.to_string_lossy()),
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

    Napi::Value convert(lib_ruby_parser::Diagnostic diagnostic, Napi::Env env)
    {
        Napi::String level;
        switch (diagnostic.level)
        {
        case lib_ruby_parser::ErrorLevel::WARNING:
            level = Napi::String::New(env, "warning");
            break;
        case lib_ruby_parser::ErrorLevel::ERROR:
            level = Napi::String::New(env, "error");
            break;
        }
        return DiagnosticCtor.New({
            level,
            Napi::String::New(env, diagnostic.message),
            convert(std::move(diagnostic.range), env),
        });
    }

    Napi::Value convert(std::vector<lib_ruby_parser::Diagnostic> diagnostics, Napi::Env env)
    {
        Napi::Array arr = Napi::Array::New(env, diagnostics.size());
        for (size_t i = 0; i < diagnostics.size(); i++)
        {
            arr.Set(i, convert(std::move(diagnostics[i]), env));
        }
        return arr;
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
        return CommentCtor.New({
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
            Napi::String::New(env, result->input.to_string_lossy()),
        });
    }

    Napi::Value convert(lib_ruby_parser::Bytes bytes, Napi::Env env)
    {
        Napi::Array array = Napi::Array::New(env, bytes.size());
        for (size_t i = 0; i < bytes.size(); i++)
        {
            array.Set(i, Napi::Number::New(env, bytes.at(i)));
        }
        return array;
    }

    void InitCustomTypes(Napi::Env env, Napi::Object exports)
    {
        Napi::Function fn;

        fn = Napi::Function::New(env, LocCtorFn, "Loc");
        LocCtor = Napi::Persistent(fn);
        LocCtor.SuppressDestruct();
        exports.Set("Loc", fn);

        fn = Napi::Function::New(env, RangeCtorFn, "Range");
        RangeCtor = Napi::Persistent(fn);
        RangeCtor.SuppressDestruct();
        exports.Set("Range", fn);

        fn = Napi::Function::New(env, TokenCtorFn, "Token");
        TokenCtor = Napi::Persistent(fn);
        TokenCtor.SuppressDestruct();
        exports.Set("Token", fn);

        fn = Napi::Function::New(env, DiagnosticCtorFn, "Diagnostic");
        DiagnosticCtor = Napi::Persistent(fn);
        DiagnosticCtor.SuppressDestruct();
        exports.Set("Diagnostic", fn);

        fn = Napi::Function::New(env, CommentCtorFn, "Comment");
        CommentCtor = Napi::Persistent(fn);
        CommentCtor.SuppressDestruct();
        exports.Set("Comment", fn);

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

#endif // LIB_RUBY_PARSER_CONVERT_H
