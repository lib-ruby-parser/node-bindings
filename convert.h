#ifndef LIB_RUBY_PARSER_CONVERT_H
#define LIB_RUBY_PARSER_CONVERT_H

#include <napi.h>
#include "lib-ruby-parser.h"
#include "convert_gen.h"

using namespace lib_ruby_parser;
using namespace Napi;

namespace lib_ruby_parser_node
{
    FunctionReference LocCtor;
    FunctionReference RangeCtor;
    FunctionReference TokenCtor;
    FunctionReference DiagnosticCtor;
    FunctionReference CommentCtor;
    FunctionReference MagicCommentCtor;
    FunctionReference ParserResultCtor;

    Value LocCtorFn(const CallbackInfo &info)
    {
        Object self = info.This().As<Object>();
        Env env = info.Env();
        self.Set("begin", info[0]);
        self.Set("end", info[1]);
        return env.Null();
    }

    Value RangeCtorFn(const CallbackInfo &info)
    {
        Object self = info.This().As<Object>();
        Env env = info.Env();
        self.Set("begin_pos", info[0]);
        self.Set("end_pos", info[1]);
        return env.Null();
    }

    Value TokenCtorFn(const CallbackInfo &info)
    {
        Object self = info.This().As<Object>();
        Env env = info.Env();
        self.Set("name", info[0]);
        self.Set("value", info[1]);
        self.Set("loc", info[2]);
        return env.Null();
    }

    Value DiagnosticCtorFn(const CallbackInfo &info)
    {
        Object self = info.This().As<Object>();
        Env env = info.Env();
        self.Set("level", info[0]);
        self.Set("message", info[1]);
        self.Set("range", info[2]);
        return env.Null();
    }

    Value CommentCtorFn(const CallbackInfo &info)
    {
        Object self = info.This().As<Object>();
        Env env = info.Env();
        self.Set("kind", info[0]);
        self.Set("location", info[1]);
        return env.Null();
    }

    Value MagicCommentCtorFn(const CallbackInfo &info)
    {
        Object self = info.This().As<Object>();
        Env env = info.Env();
        self.Set("kind", info[0]);
        self.Set("key_l", info[1]);
        self.Set("value_l", info[2]);
        return env.Null();
    }

    Value ParserResultCtorFn(const CallbackInfo &info)
    {
        Object self = info.This().As<Object>();
        Env env = info.Env();
        self.Set("ast", info[0]);
        self.Set("tokens", info[1]);
        self.Set("diagnostics", info[2]);
        self.Set("comments", info[3]);
        self.Set("magic_comments", info[4]);
        self.Set("input", info[5]);
        return env.Null();
    }

    Value convert(std::unique_ptr<Loc> loc, Env env)
    {
        if (!loc)
        {
            return env.Null();
        }
        return LocCtor.New({Value::From(env, loc->begin),
                            Value::From(env, loc->end)});
    }

    Value convert(std::unique_ptr<Range> range, Env env)
    {
        if (!range)
        {
            return env.Null();
        }
        return RangeCtor.New({
            Value::From(env, range->begin_pos),
            Value::From(env, range->end_pos),
        });
    }

    Value convert(Token token, Env env)
    {
        return TokenCtor.New({
            Value::From(env, token.name()),
            Value::From(env, token.token_value),
            convert(std::move(token.loc), env),
        });
    }

    Value convert(std::vector<Token> tokens, Env env)
    {
        Napi::Array arr = Napi::Array::New(env, tokens.size());
        for (size_t i = 0; i < tokens.size(); i++)
        {
            arr.Set(i, convert(std::move(tokens[i]), env));
        }
        return arr;
    }

    Value convert(Diagnostic diagnostic, Env env)
    {
        String level;
        switch (diagnostic.level)
        {
        case ErrorLevel::WARNING:
            level = String::New(env, "warning");
            break;
        case ErrorLevel::ERROR:
            level = String::New(env, "error");
            break;
        }
        return DiagnosticCtor.New({
            level,
            String::New(env, diagnostic.message),
            convert(std::move(diagnostic.range), env),
        });
    }

    Value convert(std::vector<Diagnostic> diagnostics, Env env)
    {
        Napi::Array arr = Napi::Array::New(env, diagnostics.size());
        for (size_t i = 0; i < diagnostics.size(); i++)
        {
            arr.Set(i, convert(std::move(diagnostics[i]), env));
        }
        return arr;
    }

    Value convert(Comment comment, Env env)
    {
        String kind;
        switch (comment.kind)
        {
        case CommentType::INLINE:
            kind = String::New(env, "inline");
            break;
        case CommentType::DOCUMENT:
            kind = String::New(env, "document");
            break;
        case CommentType::UNKNOWN:
            kind = String::New(env, "unknown");
            break;
        }
        return CommentCtor.New({
            kind,
            convert(std::move(comment.location), env),
        });
    }

    Value convert(std::vector<Comment> comments, Env env)
    {
        Napi::Array arr = Napi::Array::New(env, comments.size());
        for (size_t i = 0; i < comments.size(); i++)
        {
            arr.Set(i, convert(std::move(comments[i]), env));
        }
        return arr;
    }

    Value convert(MagicComment magic_comment, Env env)
    {
        String kind;
        switch (magic_comment.kind)
        {
        case MagicCommentKind::ENCODING:
            kind = String::New(env, "encoding");
            break;
        case MagicCommentKind::FROZEN_STRING_LITERAL:
            kind = String::New(env, "frozen-string-literal");
            break;
        case MagicCommentKind::WARN_INDENT:
            kind = String::New(env, "warn-indent");
            break;
        case MagicCommentKind::SHAREABLE_CONSTANT_VALUE:
            kind = String::New(env, "shareable-constant-value");
            break;
        }
        return MagicCommentCtor.New({
            kind,
            convert(std::move(magic_comment.key_l), env),
            convert(std::move(magic_comment.value_l), env),
        });
    }

    Value convert(std::vector<MagicComment> magic_comments, Env env)
    {
        Napi::Array arr = Napi::Array::New(env, magic_comments.size());
        for (size_t i = 0; i < magic_comments.size(); i++)
        {
            arr.Set(i, convert(std::move(magic_comments[i]), env));
        }
        return arr;
    }

    Value convert(std::unique_ptr<ParserResult> result, Env env)
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
            String::New(env, result->input),
        });
    }

    void InitCustomTypes(Env env, Object exports)
    {
        Napi::Function fn;

        fn = Function::New(env, LocCtorFn, "Loc");
        LocCtor = Persistent(fn);
        LocCtor.SuppressDestruct();
        exports.Set("Loc", fn);

        fn = Function::New(env, RangeCtorFn, "Range");
        RangeCtor = Persistent(fn);
        RangeCtor.SuppressDestruct();
        exports.Set("Range", fn);

        fn = Function::New(env, TokenCtorFn, "Token");
        TokenCtor = Persistent(fn);
        TokenCtor.SuppressDestruct();
        exports.Set("Token", fn);

        fn = Function::New(env, DiagnosticCtorFn, "Diagnostic");
        DiagnosticCtor = Persistent(fn);
        DiagnosticCtor.SuppressDestruct();
        exports.Set("Diagnostic", fn);

        fn = Function::New(env, CommentCtorFn, "Comment");
        CommentCtor = Persistent(fn);
        CommentCtor.SuppressDestruct();
        exports.Set("Comment", fn);

        fn = Function::New(env, MagicCommentCtorFn, "MagicComment");
        MagicCommentCtor = Persistent(fn);
        MagicCommentCtor.SuppressDestruct();
        exports.Set("MagicComment", fn);

        fn = Function::New(env, ParserResultCtorFn, "ParserResult");
        ParserResultCtor = Persistent(fn);
        ParserResultCtor.SuppressDestruct();
        exports.Set("ParserResult", fn);
    }
} // namespace lib_ruby_parser_node

#endif // LIB_RUBY_PARSER_CONVERT_H
