pub(crate) struct MessageH<'a> {
    _messages: &'a [lib_ruby_parser_nodes::Message],
}

impl<'a> MessageH<'a> {
    pub(crate) fn new(_messages: &'a [lib_ruby_parser_nodes::Message]) -> Self {
        Self { _messages }
    }

    pub(crate) fn write(&self) {
        std::fs::write("../src/message.h", self.contents()).unwrap();
    }

    fn contents(&self) -> String {
        format!(
            "#ifndef LIB_RUBY_PARSER_NODE_MESSAGE_H
#define LIB_RUBY_PARSER_NODE_MESSAGE_H

#include <napi.h>
#include \"lib-ruby-parser.h\"

namespace lib_ruby_parser_node
{{

    Napi::Value convert(std::unique_ptr<lib_ruby_parser::DiagnosticMessage> node, Napi::Env env);
    void InitMessageTypes(Napi::Env env, Napi::Object exports);

}} // namespace lib_ruby_parser_node

#endif // LIB_RUBY_PARSER_NODE_MESSAGE_H
"
        )
    }
}
