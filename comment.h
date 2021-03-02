#ifndef LIB_RUBY_PARSER_NODE_COMMENT_H
#define LIB_RUBY_PARSER_NODE_COMMENT_H

#include <napi.h>
#include "lib-ruby-parser.h"

namespace lib_ruby_parser_node
{
    class Comment : public Napi::ObjectWrap<Comment>
    {
    public:
        static Napi::FunctionReference ctor;

        static void Init(Napi::Env &env, Napi::Object &exports);

        Comment(const Napi::CallbackInfo &info);
    };

    Napi::Value convert(lib_ruby_parser::Comment comment, Napi::Env env);
    Napi::Value convert(std::vector<lib_ruby_parser::Comment> comments, Napi::Env env);
}

#endif // LIB_RUBY_PARSER_NODE_COMMENT_H
