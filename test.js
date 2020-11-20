let ruby_parser;

try {
    ruby_parser = require('./build/Release/ruby_parser.node');
} catch (err) {
    ruby_parser = require('./build/Debug/ruby_parser.node');
}

console.log(ruby_parser)
// console.log(typeof (ruby_parser.parse(42)));
console.log(ruby_parser.parse("foo"))
