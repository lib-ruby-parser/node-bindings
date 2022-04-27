pub(crate) struct NodeHpp<'a> {
    _nodes: &'a [lib_ruby_parser_nodes::Node],
}

impl<'a> NodeHpp<'a> {
    pub(crate) fn new(_nodes: &'a [lib_ruby_parser_nodes::Node]) -> Self {
        Self { _nodes }
    }

    pub(crate) fn write(&self) {
        std::fs::write("../src/node.hpp", self.contents()).unwrap();
    }

    fn contents(&self) -> String {
        format!(
            "#ifndef LIB_RUBY_PARSER_NODE_NODE_H
#define LIB_RUBY_PARSER_NODE_NODE_H

#include <napi.h>
#include \"lib-ruby-parser.hpp\"

namespace lib_ruby_parser_node
{{

    Napi::Value convert(std::unique_ptr<lib_ruby_parser::Node> node, Napi::Env env);
    void InitNodeTypes(Napi::Env env, Napi::Object exports);

}} // namespace lib_ruby_parser_node

#endif // LIB_RUBY_PARSER_NODE_NODE_H
"
        )
    }
}
