#ifndef LIB_RUBY_PARSER_CONVERT_H
#define LIB_RUBY_PARSER_CONVERT_H

#include <napi.h>
#include "lib-ruby-parser.h"
#include "convert_gen.h"

using namespace lib_ruby_parser;
using namespace Napi;

namespace lib_ruby_parser_node
{
    Value convert(std::unique_ptr<Loc> loc, Env env)
    {
        if (!loc)
        {
            return env.Null();
        }
        Object obj = Object::New(env);
        obj.Set("begin", loc->begin);
        obj.Set("end", loc->end);
        return obj;
    }

    Value convert(std::unique_ptr<Range> range, Env env)
    {
        if (!range)
        {
            return env.Null();
        }
        Object obj = Object::New(env);
        obj.Set("begin", range->begin_pos);
        obj.Set("end", range->end_pos);
        return obj;
    }

    Value convert(Token token, Env env)
    {
        Object obj = Object::New(env);
        obj.Set("token_type", token.token_type);
        obj.Set("token_value", String::New(env, token.token_value));
        obj.Set("loc", convert(std::move(token.loc), env));
        return obj;
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
        Object obj = Object::New(env);
        switch (diagnostic.level)
        {
        case ErrorLevel::WARNING:
            obj.Set("level", String::New(env, "warning"));
            break;
        case ErrorLevel::ERROR:
            obj.Set("level", String::New(env, "error"));
            break;
        }
        obj.Set("message", String::New(env, diagnostic.message));
        obj.Set("range", convert(std::move(diagnostic.range), env));
        return obj;
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
        Object obj = Object::New(env);
        switch (comment.kind)
        {
        case CommentType::INLINE:
            obj.Set("kind", String::New(env, "inline"));
            break;
        case CommentType::DOCUMENT:
            obj.Set("kind", String::New(env, "document"));
            break;
        case CommentType::UNKNOWN:
            obj.Set("kind", String::New(env, "unknown"));
            break;
        }
        obj.Set("location", convert(std::move(comment.location), env));
        return obj;
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
        Object obj = Object::New(env);
        switch (magic_comment.kind)
        {
        case MagicCommentKind::ENCODING:
            obj.Set("kind", String::New(env, "encoding"));
            break;
        case MagicCommentKind::FROZEN_STRING_LITERAL:
            obj.Set("kind", String::New(env, "frozen-string-literal"));
            break;
        case MagicCommentKind::WARN_INDENT:
            obj.Set("kind", String::New(env, "warn-indent"));
            break;
        }
        obj.Set("key_l", convert(std::move(magic_comment.key_l), env));
        obj.Set("value_l", convert(std::move(magic_comment.value_l), env));
        return obj;
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
        Object obj = Object::New(env);

        auto ast = convert(std::move(result->ast), env);
        auto tokens = convert(std::move(result->tokens), env);
        auto diagnostics = convert(std::move(result->diagnostics), env);
        auto comments = convert(std::move(result->comments), env);
        auto magic_comments = convert(std::move(result->magic_comments), env);
        auto input = String::New(env, result->input);

        obj.Set("ast", ast);
        obj.Set("tokens", tokens);
        obj.Set("diagnostics", diagnostics);
        obj.Set("comments", comments);
        obj.Set("magic_comments", magic_comments);
        obj.Set("input", input);

        return obj;
    }
} // namespace lib_ruby_parser_node

#endif // LIB_RUBY_PARSER_CONVERT_H
