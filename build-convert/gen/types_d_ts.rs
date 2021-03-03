pub(crate) struct TypedDTs<'a> {
    nodes: &'a [lib_ruby_parser_nodes::Node],
    messages: &'a [lib_ruby_parser_nodes::Message],
}

impl<'a> TypedDTs<'a> {
    pub(crate) fn new(
        nodes: &'a [lib_ruby_parser_nodes::Node],
        messages: &'a [lib_ruby_parser_nodes::Message],
    ) -> Self {
        Self { nodes, messages }
    }

    pub(crate) fn write(&self) {
        std::fs::write("../types.d.ts", self.contents()).unwrap()
    }

    fn contents(&self) -> String {
        format!(
            "// lib-ruby-parser

export class Loc {{
    begin: number
    end: number

    constructor(begin: number, end: number);
    source(input: Input): Uint8Array;
}}

export class Token {{
    name: string
    value: Uint8Array
    loc: Loc
}}

export type DiagnosticLevel =
    | \"error\"
    | \"warning\";

export class Diagnostic {{
    level: DiagnosticLevel
    message: Message
    rendered: string
    loc: Loc
}}

export type CommentKind =
    | \"inline\"
    | \"document\"
    | \"unknown\";

export class Comment {{
    kind: CommentKind
    location: Loc
}}

export type MagicCommentKind =
    | \"encoding\"
    | \"frozen-string-literal\"
    | \"warn-indent\"
    | \"shareable-constant-value\";

export class MagicComment {{
    kind: MagicCommentKind
    key_l: Loc
    value_l: Loc
}}

export class Input {{}}

export class ParserResult {{
    ast: Node | null
    tokens: Token[]
    diagnostics: Diagnostic[]
    comments: Comment[]
    magic_comments: MagicComment[]
    input: Input
}}

export interface ParserOptions {{}}

// Namespace with all kinds of AST nodes
{node_ifaces}

// Namespace with all kinds of AST diagnostic messages
{message_ifaces}

export type Node =
    {nodes_sum};

export type Message =
    {messages_sum};

export function bytes_to_utf8_lossy(bytes: Uint8Array): string;
export function parse(input: Uint8Array, options: ParserOptions): ParserResult;
",
            node_ifaces = self.node_ifaces().join("\n\n"),
            message_ifaces = self.message_ifaces().join("\n\n"),
            nodes_sum = self.nodes_sum().join("\n    "),
            messages_sum = self.messages_sum().join("\n    ")
        )
    }

    fn node_ifaces(&self) -> Vec<String> {
        self.nodes.iter().map(node_iface).collect()
    }
    fn message_ifaces(&self) -> Vec<String> {
        self.messages.iter().map(message_iface).collect()
    }
    fn nodes_sum(&self) -> Vec<String> {
        self.nodes.iter().map(nodes_sum_item).collect()
    }
    fn messages_sum(&self) -> Vec<String> {
        self.messages.iter().map(messages_sum_item).collect()
    }
}

fn node_iface(node: &lib_ruby_parser_nodes::Node) -> String {
    let fields = node
        .fields
        .iter()
        .map(|f| {
            let field_type = match f.field_type {
                lib_ruby_parser_nodes::FieldType::Node => "Node",
                lib_ruby_parser_nodes::FieldType::MaybeNode => "Node | null",
                lib_ruby_parser_nodes::FieldType::Nodes => "Node[]",
                lib_ruby_parser_nodes::FieldType::Loc => "Loc",
                lib_ruby_parser_nodes::FieldType::MaybeLoc => "Loc | null",
                lib_ruby_parser_nodes::FieldType::Str => "string",
                lib_ruby_parser_nodes::FieldType::MaybeStr => "string | null",
                lib_ruby_parser_nodes::FieldType::Chars => "string | null",
                lib_ruby_parser_nodes::FieldType::StringValue => "Uint8Array",
                lib_ruby_parser_nodes::FieldType::U8 => "number",
                lib_ruby_parser_nodes::FieldType::Usize => "number",
                lib_ruby_parser_nodes::FieldType::RawString => "string",
                lib_ruby_parser_nodes::FieldType::RegexOptions => "Node",
            };
            format!(
                "{comment}
    {field_name}: {field_type};",
                comment = comment(&f.comment, 4).join("\n"),
                field_name = f.field_name,
                field_type = field_type
            )
        })
        .collect::<Vec<_>>();

    format!(
        "{comment}
export class {name} {{
{fields}
}}",
        comment = comment(&node.comment, 0).join("\n"),
        name = node.struct_name,
        fields = fields.join("\n\n")
    )
}
fn message_iface(message: &lib_ruby_parser_nodes::Message) -> String {
    let fields = message
        .fields
        .iter()
        .map(|f| {
            let field_type = match f.field_type {
                lib_ruby_parser_nodes::MessageFieldType::Str => "string",
                lib_ruby_parser_nodes::MessageFieldType::Byte => "string",
            };
            format!(
                "{comment}
    {field_name}: {field_type}",
                comment = comment(&f.comment, 4).join("\n"),
                field_name = f.name,
                field_type = field_type
            )
        })
        .collect::<Vec<_>>();

    format!(
        "{comment}
export class {name} {{
{fields}
}}",
        comment = comment(&message.comment, 0).join("\n"),
        name = message.name,
        fields = fields.join("\n\n")
    )
}
fn nodes_sum_item(node: &lib_ruby_parser_nodes::Node) -> String {
    format!("| {}", node.struct_name)
}
fn messages_sum_item(message: &lib_ruby_parser_nodes::Message) -> String {
    format!("| {}", message.name)
}

fn comment(s: &str, spaces: usize) -> Vec<String> {
    s.lines()
        .map(|l| {
            format!("{}// {}", " ".repeat(spaces), l)
                .trim_end()
                .to_owned()
        })
        .collect()
}
