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
        "if constexpr (std::is_same_v<T, std::unique_ptr<lib_ruby_parser::{class_name}>>)
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
    let fields: Vec<String> = node
        .fields
        .iter()
        .map(|f| {
            format!(
                "convert(std::move(node->{name}), env),",
                name = field_name(f)
            )
        })
        .collect();

    format!(
        "Napi::Value convert(std::unique_ptr<lib_ruby_parser::{class_name}> node, Napi::Env env) {{
        if (!node) {{
            return env.Null();
        }}
        return {class_name}Ctor.New({{
            {fields}
        }});
    }}",
        class_name = node.struct_name,
        fields = fields.join("\n            ")
    )
}

fn ctor_definition(node: &Node) -> String {
    format!("Napi::FunctionReference {}Ctor;", node.struct_name)
}

fn ctor_fn_definition(node: &Node) -> String {
    let set_fields: Vec<String> = node
        .fields
        .iter()
        .enumerate()
        .map(|(idx, f)| {
            format!(
                "self.Set(\"{name}\", info[{idx}]);",
                name = field_name(f),
                idx = idx
            )
        })
        .collect();

    format!(
        "Napi::Value {name}CtorFn(const Napi::CallbackInfo &info)
    {{
        Napi::Object self = info.This().As<Napi::Object>();
        Napi::Env env = info.Env();
        {set_fields}
        return env.Null();
    }}
",
        name = node.struct_name,
        set_fields = set_fields.join("\n        ")
    )
}

fn init_exports(node: &Node) -> String {
    format!(
        "fn = Napi::Function::New(env, {name}CtorFn, \"{name}\");
        {name}Ctor = Napi::Persistent(fn);
        {name}Ctor.SuppressDestruct();
        exports.Set(\"{name}\", fn);
",
        name = node.struct_name
    )
}

fn main() {
    let path = relative_path("../convert_gen.h");
    let nodes = lib_ruby_parser_nodes::nodes().unwrap();

    let ctor_definitions: Vec<String> = nodes.iter().map(ctor_definition).collect();
    let ctor_fn_definitions: Vec<String> = nodes.iter().map(ctor_fn_definition).collect();

    let converters: Vec<String> = nodes.iter().map(build_converter).collect();
    let comparisons: Vec<String> = nodes.iter().map(build_comparison).collect();
    let init_exports: Vec<String> = nodes.iter().map(init_exports).collect();

    let contents = format!(
        "#ifndef LIB_RUBY_PARSER_CONVERT_GEN_H
#define LIB_RUBY_PARSER_CONVERT_GEN_H

#include <napi.h>
#include \"lib-ruby-parser.h\"

template<class> inline constexpr bool always_false_v = false;

namespace lib_ruby_parser_node
{{
    {ctor_definitions}
    {ctor_fn_definitions}

    Napi::Value convert(std::unique_ptr<lib_ruby_parser::Node> node, Napi::Env env);
    Napi::Value convert(lib_ruby_parser::Node node, Napi::Env env);
    Napi::Value convert(std::unique_ptr<lib_ruby_parser::Range> range, Napi::Env env);
    Napi::Value convert(lib_ruby_parser::Bytes bytes, Napi::Env env);

    Napi::Value convert(std::string s, Napi::Env env)
    {{
        return Napi::String::New(env, s);
    }}

    Napi::Value convert(size_t n, Napi::Env env)
    {{
        return Napi::Number::New(env, 0);
    }}

    Napi::Value convert(std::vector<lib_ruby_parser::Node> nodes, Napi::Env env)
    {{
        Napi::Array arr = Napi::Array::New(env, nodes.size());
        for (size_t i = 0; i < nodes.size(); i++)
        {{
            arr.Set(i, convert(std::move(nodes[i]), env));
        }}
        return arr;
    }}

    {converters}

    Napi::Value convert(std::unique_ptr<lib_ruby_parser::Node> node, Napi::Env env)
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

    Napi::Value convert(lib_ruby_parser::Node node, Napi::Env env)
    {{
        return std::visit([env](auto &&inner) {{
            using T = std::decay_t<decltype(inner)>;
            {comparisons}
            else
                static_assert(always_false_v<T>, \"non-exhaustive visitor!\");
        }}, node.inner);
    }}

    void InitNodeTypes(Napi::Env env, Napi::Object exports) {{
        Napi::Function fn;

        {init_exports}
    }}
}} // namespace lib_ruby_parser_node

#endif // LIB_RUBY_PARSER_CONVERT_GEN_H
",
        converters = converters.join("\n    "),
        comparisons = comparisons.join("\n            else "),
        ctor_definitions = ctor_definitions.join("\n    "),
        ctor_fn_definitions = ctor_fn_definitions.join("\n    "),
        init_exports = init_exports.join("\n        ")
    );

    std::fs::write(&path, &contents).unwrap();
}
