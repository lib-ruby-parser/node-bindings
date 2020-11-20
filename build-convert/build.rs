extern crate lib_ruby_parser_nodes;

use lib_ruby_parser_nodes::{Field, Node};
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

fn build_comparison(node: &Node) -> String {
    format!(
        "if constexpr (std::is_same_v<T, std::unique_ptr<{class_name}>>)
                return convert(std::move(inner), env);",
        class_name = node.struct_name
    )
}

fn field_name(f: &Field) -> String {
    match &f.field_name[..] {
        "default" => "default_",
        "operator" => "operator_",
        other => other,
    }
    .to_owned()
}

fn build_converter(node: &Node) -> String {
    let set_fields: Vec<String> = node
        .fields
        .iter()
        .map(|f| {
            format!(
                "obj.Set(\"{name}\", convert(std::move(node->{name}), env));",
                name = field_name(f)
            )
        })
        .collect();

    format!(
        "Napi::Value convert(std::unique_ptr<{class_name}> node, Napi::Env env) {{
        if (!node) {{
            return env.Null();
        }}
        Napi::Object obj = Napi::Object::New(env);
        obj.Set(\"type\", \"{class_name}\");
        {set_fields}
        return obj;
    }}",
        class_name = node.struct_name,
        set_fields = set_fields.join("\n        ")
    )
}

fn main() {
    let path = relative_path("../convert_gen.h");
    let nodes = lib_ruby_parser_nodes::nodes().unwrap();

    let converters: Vec<String> = nodes.iter().map(build_converter).collect();
    let comparisons: Vec<String> = nodes.iter().map(build_comparison).collect();

    let contents = format!(
        "#ifndef LIB_RUBY_PARSER_CONVERT_GEN_H
#define LIB_RUBY_PARSER_CONVERT_GEN_H

#include <napi.h>
#include <iostream>
#include \"lib-ruby-parser.h\"

using namespace lib_ruby_parser;

template<class> inline constexpr bool always_false_v = false;

namespace lib_ruby_parser_node
{{
    Napi::Value convert(std::unique_ptr<Node> node, Napi::Env env);
    Napi::Value convert(Node node, Napi::Env env);
    Napi::Value convert(std::unique_ptr<Range> range, Napi::Env env);

    Napi::Value convert(std::string s, Napi::Env env)
    {{
        return Napi::String::New(env, s);
    }}

    Napi::Value convert(size_t n, Napi::Env env)
    {{
        return Napi::Number::New(env, 0);
    }}

    Napi::Value convert(std::vector<Node> nodes, Napi::Env env)
    {{
        Napi::Array arr = Napi::Array::New(env, nodes.size());
        for (size_t i = 0; i < nodes.size(); i++)
        {{
            arr.Set(i, convert(std::move(nodes[i]), env));
        }}
        return arr;
    }}

    {converters}

    Napi::Value convert(std::unique_ptr<Node> node, Napi::Env env)
    {{
        if (!node) {{
            return env.Null();
        }}
        return std::visit([env](auto &&inner) -> Napi::Value {{
            using T = std::decay_t<decltype(inner)>;
            {comparisons}
            else
                static_assert(always_false_v<T>, \"non-exhaustive visitor!\");
        }}, node->inner);
    }}

    Napi::Value convert(Node node, Napi::Env env)
    {{
        return std::visit([env](auto &&inner) {{
            using T = std::decay_t<decltype(inner)>;
            {comparisons}
            else
                static_assert(always_false_v<T>, \"non-exhaustive visitor!\");
        }}, node.inner);
    }}
}} // namespace lib_ruby_parser_node

#endif // LIB_RUBY_PARSER_CONVERT_GEN_H
",
        converters = converters.join("\n    "),
        comparisons = comparisons.join("\n            else "),
    );

    std::fs::write(&path, &contents).unwrap();
}
