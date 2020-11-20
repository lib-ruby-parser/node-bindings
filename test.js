#!/usr/bin/env node

let path_to_require = process.argv[2]

if (!path_to_require) {
    console.log('no argument specified, using debug build');
    path_to_require = './build/Debug/ruby_parser.node';
}

console.log(`requiring ${path_to_require}`);
const ruby_parser = require(path_to_require);

console.log(ruby_parser)
// console.log(typeof (ruby_parser.parse(42)));
console.log(ruby_parser.parse("foo"))
