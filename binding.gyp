{
    "targets": [
        {
            "target_name": "ruby_parser",
            "sources": [ "lib-ruby-parser.cc" ],
            "include_dirs": [
                "<!@(node -p \"require('node-addon-api').include\")",
                "lib-ruby-parser-cpp-bindings/includes"
            ],
            "libraries": [ "../lib-ruby-parser-cpp-bindings/target/lib-ruby-parser-static-release" ],
            "cflags_cc": [ "-std=c++17" ],
            "xcode_settings": {
                "OTHER_CFLAGS": [ "-std=c++17"],
            },
            "defines": [ "NAPI_DISABLE_CPP_EXCEPTIONS" ],
        }
    ]
}
