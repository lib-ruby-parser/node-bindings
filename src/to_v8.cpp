#include <iostream>
#include "to_v8.hpp"
#include "lib-ruby-parser.hpp"

void log_with_env(Napi::Value &value, Napi::Env &env)
{
    Napi::Object global = env.Global();
    Napi::Object console = global.Get("console").As<Napi::Object>();
    Napi::Function log = console.Get("log").As<Napi::Function>();
    log.Call({value});
}

decl_to_v8_for(lib_ruby_parser::ParserResult);
decl_to_v8_for(lib_ruby_parser::TokenList);
decl_to_v8_for(lib_ruby_parser::Token);
decl_to_v8_for(lib_ruby_parser::Bytes);
decl_to_v8_for(lib_ruby_parser::Loc);
decl_to_v8_for(int32_t);
decl_to_v8_for(size_t);
decl_to_v8_for(lib_ruby_parser::CommentList);
decl_to_v8_for(lib_ruby_parser::Comment);
decl_to_v8_for(lib_ruby_parser::CommentType);
decl_to_v8_for(lib_ruby_parser::MagicCommentList);
decl_to_v8_for(lib_ruby_parser::MagicComment);
decl_to_v8_for(lib_ruby_parser::MagicCommentKind);
decl_to_v8_for(lib_ruby_parser::DecodedInput);
decl_to_v8_for(lib_ruby_parser::String);
decl_to_v8_for(lib_ruby_parser::SourceLineList);
decl_to_v8_for(lib_ruby_parser::SourceLine);
decl_to_v8_for(bool);
decl_to_v8_for(lib_ruby_parser::ByteList);
decl_to_v8_for(lib_ruby_parser::DiagnosticList);
decl_to_v8_for(lib_ruby_parser::Diagnostic);
decl_to_v8_for(lib_ruby_parser::ErrorLevel);

template <>
Napi::Value ToV8<lib_ruby_parser::ParserResult>(lib_ruby_parser::ParserResult value,
                                                Napi::Env &env,
                                                Napi::Object &objects)
{
    dbg("ToV8(ParserResult)");
    GetObject(ParserResult);
    return ParserResult.New({
        ToV8(std::move(value.ast), env, objects),
        ToV8(std::move(value.tokens), env, objects),
        ToV8(std::move(value.diagnostics), env, objects),
        ToV8(std::move(value.comments), env, objects),
        ToV8(std::move(value.magic_comments), env, objects),
        ToV8(std::move(value.input), env, objects),
    });
}

// Node implementations are in nodes.cpp

template <>
Napi::Value ToV8<lib_ruby_parser::DiagnosticList>(lib_ruby_parser::DiagnosticList value, Napi::Env &env, Napi::Object &objects)
{
    dbg("ToV8(DiagnosticList)");
    Napi::Array arr = Napi::Array::New(env, value.len);
    for (size_t idx = 0; idx < value.len; idx++)
    {
        arr.Set(idx, ToV8(std::move(value.ptr[idx]), env, objects));
    }
    return arr;
}

template <>
Napi::Value ToV8<lib_ruby_parser::Diagnostic>(lib_ruby_parser::Diagnostic value, Napi::Env &env, Napi::Object &objects)
{
    dbg("ToV8(Diagnostic)");
    GetObject(Diagnostic);
    return Diagnostic.New({
        ToV8(value.level, env, objects),
        ToV8(std::move(value.message), env, objects),
        ToV8(value.loc, env, objects),
    });
}

// DiagnosticMessage implementations are in messages.cpp

template <>
Napi::Value ToV8<lib_ruby_parser::ErrorLevel>(lib_ruby_parser::ErrorLevel value, Napi::Env &env, Napi::Object &objects)
{
    dbg("ToV8(ErrorLevel)");
    switch (value)
    {
    case lib_ruby_parser::ErrorLevel::WARNING:
        return Napi::String::New(env, "warning");
    case lib_ruby_parser::ErrorLevel::ERROR:
        return Napi::String::New(env, "error");
    default:
        return Napi::String::New(env, "<INTERNAL ERROR, PLEASE REPORT A BUG>");
    }
}

// TokenList start
template <>
Napi::Value ToV8<lib_ruby_parser::TokenList>(lib_ruby_parser::TokenList value, Napi::Env &env, Napi::Object &objects)
{
    dbg("ToV8(TokenList)");
    Napi::Array arr = Napi::Array::New(env, value.len);
    for (size_t idx = 0; idx < value.len; idx++)
    {
        arr.Set(idx, ToV8(std::move(value.ptr[idx]), env, objects));
    }
    return arr;
}

template <>
Napi::Value ToV8<lib_ruby_parser::Token>(lib_ruby_parser::Token value, Napi::Env &env, Napi::Object &objects)
{
    dbg("ToV8(Token)");
    GetObject(Token);
    return Token.New({
        ToV8(value.token_type, env, objects),
        ToV8(std::move(value.token_value), env, objects),
        ToV8(value.loc, env, objects),
        ToV8(value.lex_state_before, env, objects),
        ToV8(value.lex_state_after, env, objects),
    });
}

template <>
Napi::Value ToV8<lib_ruby_parser::Bytes>(lib_ruby_parser::Bytes value, Napi::Env &env, Napi::Object &objects)
{
    dbg("ToV8(Bytes)");
    Napi::Uint8Array arr = Napi::Uint8Array::New(env, value.raw.len);
    for (size_t idx = 0; idx < value.raw.len; idx++)
    {
        arr[idx] = value.raw.ptr[idx];
    }
    return arr;
}

template <>
Napi::Value ToV8<lib_ruby_parser::Loc>(lib_ruby_parser::Loc value, Napi::Env &env, Napi::Object &objects)
{
    dbg("ToV8(Loc)");
    GetObject(Loc);
    return Loc.New({
        ToV8(value.begin, env, objects),
        ToV8(value.end, env, objects),
    });
}

template <>
Napi::Value ToV8<lib_ruby_parser::MaybeLoc>(lib_ruby_parser::MaybeLoc value, Napi::Env &env, Napi::Object &objects)
{
    dbg("ToV8(MaybeLoc)");
    if (value.tag == lib_ruby_parser::MaybeLoc::Tag::NONE)
    {
        return env.Null();
    }
    else
    {
        return ToV8(value.as.loc, env, objects);
    }
}

template <>
Napi::Value ToV8<size_t>(size_t value, Napi::Env &env, Napi::Object &objects)
{
    dbg("ToV8(size_t)");
    return Napi::Number::New(env, value);
}

template <>
Napi::Value ToV8<int32_t>(int32_t value, Napi::Env &env, Napi::Object &objects)
{
    dbg("ToV8(int32_t)");
    return Napi::Number::New(env, value);
}

// TokenList end

// CommentList start

template <>
Napi::Value ToV8<lib_ruby_parser::CommentList>(lib_ruby_parser::CommentList value, Napi::Env &env, Napi::Object &objects)
{
    dbg("ToV8(CommentList)");
    Napi::Array arr = Napi::Array::New(env, value.len);
    for (size_t idx = 0; idx < value.len; idx++)
    {
        arr.Set(idx, ToV8(std::move(value.ptr[idx]), env, objects));
    }
    return arr;
}

template <>
Napi::Value ToV8<lib_ruby_parser::Comment>(lib_ruby_parser::Comment value, Napi::Env &env, Napi::Object &objects)
{
    dbg("ToV8(Comment)");
    GetObject(Comment);
    return Comment.New({
        ToV8(value.location, env, objects),
        ToV8(value.kind, env, objects),
    });
}

template <>
Napi::Value ToV8<lib_ruby_parser::CommentType>(lib_ruby_parser::CommentType value, Napi::Env &env, Napi::Object &objects)
{
    dbg("ToV8(CommentType)");
    switch (value)
    {
    case lib_ruby_parser::CommentType::INLINE:
        return Napi::String::New(env, "inline");
    case lib_ruby_parser::CommentType::DOCUMENT:
        return Napi::String::New(env, "document");
    case lib_ruby_parser::CommentType::UNKNOWN:
        return Napi::String::New(env, "unknown");
    default:
        return Napi::String::New(env, "<INTERNAL ERROR, PLEASE REPORT A BUG>");
    }
}

// CommentList end

// MagicCommentList start

template <>
Napi::Value ToV8<lib_ruby_parser::MagicCommentList>(lib_ruby_parser::MagicCommentList value, Napi::Env &env, Napi::Object &objects)
{
    dbg("ToV8(MagicCommentList)");
    Napi::Array arr = Napi::Array::New(env, value.len);
    for (size_t idx = 0; idx < value.len; idx++)
    {
        arr.Set(idx, ToV8(std::move(value.ptr[idx]), env, objects));
    }
    return arr;
}

template <>
Napi::Value ToV8<lib_ruby_parser::MagicComment>(lib_ruby_parser::MagicComment value, Napi::Env &env, Napi::Object &objects)
{
    dbg("ToV8(MagicComment)");
    GetObject(MagicComment);
    return MagicComment.New({
        ToV8(value.kind, env, objects),
        ToV8(value.key_l, env, objects),
        ToV8(value.value_l, env, objects),
    });
}

template <>
Napi::Value ToV8<lib_ruby_parser::MagicCommentKind>(lib_ruby_parser::MagicCommentKind value, Napi::Env &env, Napi::Object &objects)
{
    dbg("ToV8(MagicCommentKind)");
    switch (value)
    {
    case lib_ruby_parser::MagicCommentKind::ENCODING:
        return Napi::String::New(env, "encoding");
    case lib_ruby_parser::MagicCommentKind::FROZEN_STRING_LITERAL:
        return Napi::String::New(env, "frozen_string_literal");
    case lib_ruby_parser::MagicCommentKind::WARN_INDENT:
        return Napi::String::New(env, "warn_indent");
    case lib_ruby_parser::MagicCommentKind::SHAREABLE_CONSTANT_VALUE:
        return Napi::String::New(env, "shareable_constant_value");
    default:
        return Napi::String::New(env, "<INTERNAL ERROR, PLEASE REPORT A BUG>");
    }
}

// MagicCommentList end

// Input start

template <>
Napi::Value ToV8<lib_ruby_parser::DecodedInput>(lib_ruby_parser::DecodedInput value, Napi::Env &env, Napi::Object &objects)
{
    dbg("ToV8(DecodedInput)");
    GetObject(DecodedInput);
    return DecodedInput.New({
        ToV8(std::move(value.name), env, objects),
        ToV8(std::move(value.lines), env, objects),
        ToV8(std::move(value.bytes), env, objects),
    });
}

template <>
Napi::Value ToV8<lib_ruby_parser::String>(lib_ruby_parser::String value, Napi::Env &env, Napi::Object &objects)
{
    dbg("ToV8(String)");
    return Napi::String::New(env, value.ptr, value.len);
}

template <>
Napi::Value ToV8<lib_ruby_parser::MaybeString>(lib_ruby_parser::MaybeString value, Napi::Env &env, Napi::Object &objects)
{
    dbg("ToV8(MaybeString)");
    if (value.is_some())
    {
        return Napi::String::New(env, value.string.ptr, value.string.len);
    }
    else
    {
        return env.Null();
    }
}

template <>
Napi::Value ToV8<lib_ruby_parser::SourceLineList>(lib_ruby_parser::SourceLineList value, Napi::Env &env, Napi::Object &objects)
{
    dbg("ToV8(SourceLineList)");
    Napi::Array arr = Napi::Array::New(env, value.len);
    for (size_t idx = 0; idx < value.len; idx++)
    {
        arr.Set(idx, ToV8(std::move(value.ptr[idx]), env, objects));
    }
    return arr;
}

template <>
Napi::Value ToV8<lib_ruby_parser::SourceLine>(lib_ruby_parser::SourceLine value, Napi::Env &env, Napi::Object &objects)
{
    dbg("ToV8(SourceLine)");
    GetObject(SourceLine);
    return SourceLine.New({
        ToV8(value.start, env, objects),
        ToV8(value.end, env, objects),
        ToV8(value.ends_with_eof, env, objects),
    });
    return env.Null();
}

template <>
Napi::Value ToV8<bool>(bool value, Napi::Env &env, Napi::Object &objects)
{
    dbg("ToV8(bool)");
    return Napi::Boolean::New(env, value);
}

template <>
Napi::Value ToV8<lib_ruby_parser::ByteList>(lib_ruby_parser::ByteList value, Napi::Env &env, Napi::Object &objects)
{
    dbg("ToV8(ByteList)");
    Napi::Uint8Array arr = Napi::Uint8Array::New(env, value.len);
    for (size_t idx = 0; idx < value.len; idx++)
    {
        arr[idx] = value.ptr[idx];
    }
    return arr;
}

// Input end

template <>
Napi::Value ToV8<uint8_t>(uint8_t value, Napi::Env &env, Napi::Object &objects)
{
    dbg("ToV8(uint8_t)");
    return Napi::Number::New(env, value);
}

template <>
Napi::Value ToV8<lib_ruby_parser::NodeList>(lib_ruby_parser::NodeList value, Napi::Env &env, Napi::Object &objects)
{
    dbg("ToV8(NodeList)");
    Napi::Array arr = Napi::Array::New(env, value.len);
    for (size_t idx = 0; idx < value.len; idx++)
    {
        arr.Set(idx, ToV8(std::move(value.ptr[idx]), env, objects));
    }
    return arr;
}
