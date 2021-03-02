#ifndef LIB_RUBY_PARSER_NODE_CONVERT_H
#define LIB_RUBY_PARSER_NODE_CONVERT_H

#include <napi.h>
#include "lib-ruby-parser.h"
#include "bytes.h"
#include "input.h"
#include "loc.h"
#include "token.h"
#include "diagnostic.h"
#include "comment.h"
#include "magic_comment.h"
#include "parser_result.h"

namespace lib_ruby_parser_node
{
    void InitCustomTypes(Napi::Env env, Napi::Object exports)
    {
        Loc::Init(env, exports);
        Token::Init(env, exports);
        Diagnostic::Init(env, exports);
        Comment::Init(env, exports);
        MagicComment::Init(env, exports);
        ParserResult::Init(env, exports);
    }
} // namespace lib_ruby_parser_node

#endif // LIB_RUBY_PARSER_NODE_CONVERT_H
