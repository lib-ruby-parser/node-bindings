extern crate lib_ruby_parser_nodes;

mod gen;

fn main() {
    let nodes = lib_ruby_parser_nodes::nodes();

    gen::NodeH::new(&nodes).write();
    gen::NodeCc::new(&nodes).write();

    let messages = lib_ruby_parser_nodes::messages()
        .sections
        .iter()
        .flat_map(|s| s.messages.to_owned())
        .collect::<Vec<_>>();

    gen::MessageH::new(&messages).write();
    gen::MessageCc::new(&messages).write();

    gen::TypedDTs::new(&nodes, &messages).write();
}
