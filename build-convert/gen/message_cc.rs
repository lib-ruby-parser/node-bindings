use lib_ruby_parser_nodes::MessageField;

pub(crate) struct MessageCc<'a> {
    messages: &'a [lib_ruby_parser_nodes::Message],
}

impl<'a> MessageCc<'a> {
    pub(crate) fn new(messages: &'a [lib_ruby_parser_nodes::Message]) -> Self {
        Self { messages }
    }

    pub(crate) fn write(&self) {
        std::fs::write("../src/message.cc", self.contents()).unwrap();
    }

    fn contents(&self) -> String {
        format!(
            "#include \"message.h\"
#include \"convert/string.h\"
#include \"convert/size_t.h\"

namespace lib_ruby_parser_node {{
    {ctor_declarations}

    {ctor_fn_definitions}

    {converters}

    Napi::Value convert(std::unique_ptr<lib_ruby_parser::DiagnosticMessage> diagnostic_message, Napi::Env env)
    {{
        if (!diagnostic_message) {{
            return env.Null();
        }}
        return std::visit([env](auto &&variant) -> Napi::Value {{
            using T = std::decay_t<decltype(variant)>;
            {comparisons}
        }}, diagnostic_message->variant);
    }}

    void InitMessageTypes(Napi::Env env, Napi::Object exports) {{
        Napi::Function fn;

        {init_exports}
    }}
}}

",
            ctor_declarations = self.ctor_declarations().join("\n    "),
            ctor_fn_definitions = self.ctor_fn_definitions().join("\n    "),
            converters = self.converters().join("\n    "),
            comparisons = self.comparisons().join("\n            "),
            init_exports = self.init_exports().join("\n        ")
        )
    }

    fn ctor_declarations(&self) -> Vec<String> {
        self.messages.iter().map(ctor_declaration).collect()
    }
    fn ctor_fn_definitions(&self) -> Vec<String> {
        self.messages.iter().map(ctor_fn_definition).collect()
    }
    fn converters(&self) -> Vec<String> {
        self.messages.iter().map(converter).collect()
    }
    fn comparisons(&self) -> Vec<String> {
        self.messages.iter().map(comparison).collect()
    }
    fn init_exports(&self) -> Vec<String> {
        self.messages.iter().map(init_exports).collect()
    }
}

fn ctor_declaration(message: &lib_ruby_parser_nodes::Message) -> String {
    format!("Napi::FunctionReference {}Ctor;", message.name)
}

fn field_name(f: &lib_ruby_parser_nodes::MessageField) -> String {
    match &f.name[..] {
        "default" => "default_",
        "operator" => "operator_",
        other => other,
    }
    .to_owned()
}

fn ctor_fn_definition(message: &lib_ruby_parser_nodes::Message) -> String {
    let set_fields: Vec<String> = message
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
        (void)self;
        {set_fields}
        return env.Null();
    }}
",
        name = message.name,
        set_fields = set_fields.join("\n        ")
    )
}

fn converter(message: &lib_ruby_parser_nodes::Message) -> String {
    let fields: Vec<String> = message
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
        class_name = message.name,
        fields = fields.join("\n            ")
    )
}
fn comparison(message: &lib_ruby_parser_nodes::Message) -> String {
    format!(
        "if constexpr (std::is_same_v<T, std::unique_ptr<lib_ruby_parser::{class_name}>>) {{
                return convert(std::move(variant), env);
            }}",
        class_name = message.name
    )
}

fn init_exports(message: &lib_ruby_parser_nodes::Message) -> String {
    format!(
        "fn = Napi::Function::New(env, {name}CtorFn, \"{name}\");
        {name}Ctor = Napi::Persistent(fn);
        {name}Ctor.SuppressDestruct();
        exports.Set(\"{name}\", fn);
",
        name = message.name
    )
}
