pub(crate) struct NodeCc<'a> {
    nodes: &'a [lib_ruby_parser_nodes::Node],
}

impl<'a> NodeCc<'a> {
    pub(crate) fn new(nodes: &'a [lib_ruby_parser_nodes::Node]) -> Self {
        Self { nodes }
    }

    pub(crate) fn write(&self) {
        std::fs::write("../src/node.cc", self.contents()).unwrap();
    }

    fn contents(&self) -> String {
        format!(
            "#include \"node.h\"
#include \"loc.h\"
#include \"bytes.h\"
#include \"convert/string.h\"
#include \"convert/size_t.h\"

namespace lib_ruby_parser_node
{{
    {ctor_declarations}

    {ctor_fn_definitions}

    Napi::Value convert(std::unique_ptr<lib_ruby_parser::Node> node, Napi::Env env);
    Napi::Value convert(lib_ruby_parser::Node node, Napi::Env env);

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
        }}, node->inner);
    }}

    Napi::Value convert(lib_ruby_parser::Node node, Napi::Env env)
    {{
        return std::visit([env](auto &&inner) {{
            using T = std::decay_t<decltype(inner)>;
            {comparisons}
        }}, node.inner);
    }}

    void InitNodeTypes(Napi::Env env, Napi::Object exports) {{
        Napi::Function fn;

        {init_exports}
    }}
}} // namespace lib_ruby_parser_node
",
            converters = self.converters().join("\n    "),
            comparisons = self.comparisons().join("\n            "),
            ctor_declarations = self.ctor_declarations().join("\n    "),
            ctor_fn_definitions = self.ctor_fn_definitions().join("\n    "),
            init_exports = self.init_exports().join("\n        ")
        )
    }

    fn ctor_declarations(&self) -> Vec<String> {
        self.nodes.iter().map(ctor_declaration).collect()
    }
    fn ctor_fn_definitions(&self) -> Vec<String> {
        self.nodes.iter().map(ctor_fn_definition).collect()
    }

    fn converters(&self) -> Vec<String> {
        self.nodes.iter().map(build_converter).collect()
    }
    fn comparisons(&self) -> Vec<String> {
        self.nodes.iter().map(build_comparison).collect()
    }
    fn init_exports(&self) -> Vec<String> {
        self.nodes.iter().map(init_exports).collect()
    }
}

fn build_comparison(node: &lib_ruby_parser_nodes::Node) -> String {
    format!(
        "if constexpr (std::is_same_v<T, std::unique_ptr<lib_ruby_parser::{class_name}>>) {{
                return convert(std::move(inner), env);
            }}",
        class_name = node.struct_name
    )
}

fn field_name(f: &lib_ruby_parser_nodes::Field) -> String {
    match &f.field_name[..] {
        "default" => "default_",
        "operator" => "operator_",
        other => other,
    }
    .to_owned()
}

fn build_converter(node: &lib_ruby_parser_nodes::Node) -> String {
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

fn ctor_declaration(node: &lib_ruby_parser_nodes::Node) -> String {
    format!("Napi::FunctionReference {}Ctor;", node.struct_name)
}

fn ctor_fn_definition(node: &lib_ruby_parser_nodes::Node) -> String {
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

fn init_exports(node: &lib_ruby_parser_nodes::Node) -> String {
    format!(
        "fn = Napi::Function::New(env, {name}CtorFn, \"{name}\");
        {name}Ctor = Napi::Persistent(fn);
        {name}Ctor.SuppressDestruct();
        exports.Set(\"{name}\", fn);
",
        name = node.struct_name
    )
}
