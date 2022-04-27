{
    "targets": [
        {
            "target_name": "ruby_parser",
            "sources": [
                "src/node_bindings.cpp",
                "src/loc.cpp",
                "src/input.cpp",
                "src/bytes.cpp",
                "src/token.cpp",
                "src/diagnostic.cpp",
                "src/comment.cpp",
                "src/magic_comment.cpp",
                "src/node.cpp",
                "src/parser_result.cpp",
                "src/message.cpp",
                "src/convert/size_t.cpp",
                "src/convert/string.cpp",
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
