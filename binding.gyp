{
    "targets": [
        {
            "target_name": "ruby_parser",
            "sources": [
                "src/node_bindings.cc",
                "src/loc.cc",
                "src/input.cc",
                "src/bytes.cc",
                "src/token.cc",
                "src/diagnostic.cc",
                "src/comment.cc",
                "src/magic_comment.cc",
                "src/node.cc",
                "src/parser_result.cc",
                "src/message.cc",
                "src/convert/size_t.cc",
                "src/convert/string.cc",
            ],
            "include_dirs": [
                "<!@(node -p \"require('node-addon-api').include\")",
                "src"
            ],
            "cflags_cc": [ "-std=c++17" ],
            "xcode_settings": {
                "OTHER_CFLAGS": [ "-std=c++17"],
            },
            "msvs_settings": {
                "VCCLCompilerTool": {
                    "AdditionalOptions": ["/std:c++17"]
                }
            },
            "defines": [ "NAPI_DISABLE_CPP_EXCEPTIONS" ],
            "conditions": [
                ['OS=="mac"', { "libraries": ["../lib-ruby-parser.a"] }],
                ['OS=="linux"', { "libraries": ["../lib-ruby-parser.a"] }],
                ['OS=="win"', { "libraries": ["../lib-ruby-parser.lib"] }],
            ]
        }
    ]
}
