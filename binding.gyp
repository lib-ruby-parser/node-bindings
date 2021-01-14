{
    "targets": [
        {
            "target_name": "ruby_parser",
            "sources": [ "node_bindings.cc" ],
            "include_dirs": [
                "<!@(node -p \"require('node-addon-api').include\")",
                "."
            ],
            "libraries": [ "../lib-ruby-parser.a" ],
            "cflags_cc": [ "-std=c++17" ],
            "xcode_settings": {
                "OTHER_CFLAGS": [ "-std=c++17"],
            },
            "defines": [ "NAPI_DISABLE_CPP_EXCEPTIONS" ],
        }
    ]
}
