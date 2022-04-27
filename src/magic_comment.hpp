#ifndef LIB_RUBY_PARSER_NODE_MAGIC_COMMENT_H
#define LIB_RUBY_PARSER_NODE_MAGIC_COMMENT_H

#include <napi.h>
#include "lib-ruby-parser.hpp"

namespace lib_ruby_parser_node
{
    class MagicComment : public Napi::ObjectWrap<MagicComment>
    {
    public:
        static Napi::FunctionReference ctor;

        static void Init(Napi::Env &env, Napi::Object &exports);

        MagicComment(const Napi::CallbackInfo &info);
    };

    Napi::Value convert(lib_ruby_parser::MagicComment magic_comment, Napi::Env env);
    Napi::Value convert(std::vector<lib_ruby_parser::MagicComment> magic_comments, Napi::Env env);
}

#endif // LIB_RUBY_PARSER_NODE_MAGIC_COMMENT_H
