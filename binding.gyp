{
    "targets": [
        {
            "target_name": "ruby_parser",
            "sources": [
                "src/node_bindings.cpp",
                "src/to_v8.cpp",
                "src/node_to_v8.cpp",
                "src/message_to_v8.cpp",
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
            "defines": [
                "NAPI_DISABLE_CPP_EXCEPTIONS",
                # "LIB_RUBY_PARSER_DEBUG"
            ],
            "conditions": [
                ['OS=="mac"', { "libraries": ["../lib-ruby-parser.a"] }],
                ['OS=="linux"', { "libraries": ["../lib-ruby-parser.a"] }],
                ['OS=="win"', { "libraries": ["../lib-ruby-parser.lib", "bcrypt.lib"] }],
            ]
        }
    ]
}
