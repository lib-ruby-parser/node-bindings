#!/usr/bin/env node

let path_to_require = process.argv[2]

if (!path_to_require) {
    console.log('no argument specified, using debug build')
    path_to_require = './build/Debug/ruby_parser.node'
}

console.log(`requiring ${path_to_require}`)
const { parse, Range, Send, Self_, Int, Token, Loc } = require(path_to_require)
const assert = require('assert').strict
const inspect = require('util').inspect;

function assert_throws(...args) {
    let thrown = false
    try {
        parse(...args)
    } catch (e) {
        thrown = true
    }

    assert(thrown, `expected an error to be thrown for "${args}"`)
}

function assert_node_type(node, type) {
    assert(node instanceof type, `expected node type to be ${type}, got ${node.type}`)
}

function assert_range(range, begin_pos, end_pos) {
    assert(range instanceof Range, `expected ${range} to be an instance of Range`)
    assert(range.begin_pos === begin_pos, `expected 'begin_pos' of range to be ${begin_pos}, got ${range.begin_pos}`)
    assert(range.end_pos === end_pos, `expected 'end_pos' of range to be ${end_pos}, got ${range.end_pos}`)
}

function assert_token(token, name, value, loc) {
    assert(token instanceof Token, `expected ${token} to be an instance of Token`)
    assert(token.name == name, `expected token's name to be ${name}, got ${token.name}`)
    assert(token.value == value, `expected token's value to be ${value}, got ${token.value}`)
    assert(token.loc.begin == loc.begin, `expected token's begin to be ${loc.begin}, got ${token.loc.begin}`)
    assert(token.loc.end == loc.end, `expected token's end to be ${loc.end}, got ${token.loc.end}`)
}

assert_throws(42)
assert_throws("foo", 10)

let result = parse(
    "self.foo(123)",
    {
        record_tokens: true,
        debug: true,
        buffer_name: "(test)"
    }
)
console.log(inspect(result, { showHidden: false, depth: null }));

assert(result !== null, "expected result to be non-null")

let { ast, tokens, diagnostics, comments, magic_comments, input } = result


assert_node_type(ast, Send)
let send = ast
assert_range(send.dot_l, 4, 5)
assert_range(send.selector_l, 5, 8)
assert_range(send.begin_l, 8, 9)
assert_range(send.end_l, 12, 13)
assert(send.operator_l === null)
assert_range(send.expression_l, 0, 13)

assert_node_type(send.recv, Self_)
let self = send.recv
assert_range(self.expression_l, 0, 4)

assert(send.method_name === "foo")

assert(send.args.length == 1)
assert_node_type(send.args[0], Int)
let arg = send.args[0]
assert(arg.value === "123")
assert(arg.operator_l === null)
assert_range(arg.expression_l, 9, 12)

assert(tokens.length == 7)
assert_token(tokens[0], "kSELF", "self", new Loc(0, 4))
assert_token(tokens[1], "tDOT", ".", new Loc(4, 5))
assert_token(tokens[2], "tIDENTIFIER", "foo", new Loc(5, 8))
assert_token(tokens[3], "tLPAREN2", "(", new Loc(8, 9))
assert_token(tokens[4], "tINTEGER", "123", new Loc(9, 12))
assert_token(tokens[5], "tRPAREN", ")", new Loc(12, 13))
assert_token(tokens[6], "EOF", "", new Loc(13, 13))

assert(diagnostics.length === 0)
assert(comments.length === 0)
assert(magic_comments.length === 0)
assert(input === "self.foo(123)")


