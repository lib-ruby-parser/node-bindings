#!/usr/bin/env node

let path_to_require = process.argv[2]

if (!path_to_require) {
    console.log('no argument specified, using debug build')
    path_to_require = './build/Debug/ruby_parser.node'
}

console.log(`requiring ${path_to_require}`)
const { parse } = require(path_to_require)
const assert = require('assert').strict

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
    assert(node !== null, `can't assert type of null node`)
    assert(node.type === type, `expected node type to be ${type}, got ${node.type}`)
}

function assert_range(range, begin, end) {
    assert(range !== null, `can't assert begin/end of null range`)
    assert(range.begin === begin, `expected 'begin' of range to be ${begin}, got ${range.begin}`)
    assert(range.end === end, `expected 'end' of range to be ${end}, got ${range.end}`)
}

assert_throws(42)
assert_throws("foo", 10)

let result = parse("self.foo(123)")
console.log(JSON.stringify(result, null, 4))

assert(result !== null, "expected result to be non-null")

let { ast, tokens, diagnostics, comments, magic_comments, input } = result


assert_node_type(ast, "Send")
let send = ast
assert_range(send.dot_l, 4, 5)
assert_range(send.selector_l, 5, 8)
assert_range(send.begin_l, 8, 9)
assert_range(send.end_l, 12, 13)
assert(send.operator_l === null)
assert_range(send.expression_l, 0, 13)

assert_node_type(send.recv, "Self_")
let self = send.recv
assert_range(self.expression_l, 0, 4)

assert(send.method_name === "foo")

assert(send.args.length == 1)
assert_node_type(send.args[0], "Int")
let arg = send.args[0]
assert(arg.value === "123")
assert(arg.operator_l === null)
assert_range(arg.expression_l, 9, 12)

let expected_tokens = [
    [288, "self", [0, 4]],
    [323, ".", [4, 5]],
    [307, "foo", [5, 8]],
    [391, "(", [8, 9]],
    [314, "123", [9, 12]],
    [358, ")", [12, 13]],
    [0, "", [13, 13]]
]

assert(tokens.length == expected_tokens.length)

for (let i = 0; i < tokens.length; i++) {
    assert(tokens[i].token_type == expected_tokens[i][0])
    assert(tokens[i].token_value == expected_tokens[i][1])
    assert(tokens[i].loc.begin == expected_tokens[i][2][0])
    assert(tokens[i].loc.end == expected_tokens[i][2][1])
}

assert(diagnostics.length === 0)
assert(comments.length === 0)
assert(magic_comments.length === 0)
assert(input === "self.foo(123)")


