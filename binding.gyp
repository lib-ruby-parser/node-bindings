{
    "targets": [
        {
            "target_name": "ruby_parser",
            "sources": [
                "node_bindings.cc",
                "loc.cc",
                "input.cc",
                "token.cc",
                "diagnostic.cc",
            ],
            "include_dirs": [
                "<!@(node -p \"require('node-addon-api').include\")",
                "."
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
