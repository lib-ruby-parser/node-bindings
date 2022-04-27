extern crate lib_ruby_parser_nodes;

mod gen;

fn main() {
    let nodes = lib_ruby_parser_nodes::nodes();

    gen::NodeHpp::new(&nodes).write();
    gen::NodeCpp::new(&nodes).write();

    let messages = lib_ruby_parser_nodes::messages()
        .sections
        .iter()
        .flat_map(|s| s.messages.to_owned())
        .collect::<Vec<_>>();

    gen::MessageHpp::new(&messages).write();
    gen::MessageCpp::new(&messages).write();

    gen::TypedDTs::new(&nodes, &messages).write();
}
