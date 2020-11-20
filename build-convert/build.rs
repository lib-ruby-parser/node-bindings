extern crate lib_ruby_parser_nodes;

use lib_ruby_parser_nodes::Node;
use std::path::Path;

fn relative_path(path: &str) -> String {
    Path::new(file!())
        .parent()
        .unwrap()
        .join(path)
        .to_str()
        .unwrap()
        .to_owned()
}

fn build_converter(node: &Node) -> String {
    format!("")
}

fn main() {
    let path = relative_path("../convert_gen.h");
    let nodes = lib_ruby_parser_nodes::nodes().unwrap();

    let converters: Vec<String> = nodes.iter().map(build_converter).collect();

    let contents = format!(
        "#ifndef LIB_RUBY_PARSER_CONVERT_GEN_H
#define LIB_RUBY_PARSER_CONVERT_GEN_H

#include <napi.h>
#include \"lib-ruby-parser.h\"

using namespace lib_ruby_parser;
using namespace Napi;

namespace lib_ruby_parser_node {{
    Value convert(std::unique_ptr<Node> node, Env env)
    {{
        // Object result = Object::New(env);
        return env.Null();
    }}
}}

#endif // LIB_RUBY_PARSER_CONVERT_GEN_H"
    );

    std::fs::write(&path, &contents).unwrap();
}
