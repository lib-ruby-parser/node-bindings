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
#include "magic_comment.h"

namespace lib_ruby_parser_node
{
    Napi::FunctionReference ParserResultCtor;

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

    void InitCustomTypes(Napi::Env env, Napi::Object exports)
    {
        Napi::Function fn;

        Loc::Init(env, exports);
        Token::Init(env, exports);
        Diagnostic::Init(env, exports);
        Comment::Init(env, exports);
        MagicComment::Init(env, exports);

        fn = Napi::Function::New(env, ParserResultCtorFn, "ParserResult");
        ParserResultCtor = Napi::Persistent(fn);
        ParserResultCtor.SuppressDestruct();
        exports.Set("ParserResult", fn);
    }
} // namespace lib_ruby_parser_node

#endif // LIB_RUBY_PARSER_NODE_CONVERT_H
