#!/usr/bin/env node

let path_to_require = process.argv[2]

if (!path_to_require) {
    console.log('no argument specified, using debug build')
    path_to_require = './build/Debug/ruby_parser.node'
}

console.log(`requiring ${path_to_require}`)
const { parse, Range, Send, Self_, Int, Token, Loc, Diagnostic } = require(path_to_require)
const assert = require('assert').strict
const inspect = require('util').inspect

function assert_eq(actual, expected, prefix) {
    assert(actual === expected, `[${prefix}] assertion failed: expected ${expected}, got ${actual}`)
}

function assert_array_eq(actual, expected, prefix) {
    assert_eq(actual.length, expected.length, `${prefix}.length`)
    for (let i = 0; i < actual.length; i++) {
        assert_eq(actual[i], expected[i], `${prefix}[${i}]`)
    }
}

function assert_parse_throws(...args) {
    let thrown = false
    try {
        parse(...args)
    } catch (e) {
        thrown = true
    }

    assert(thrown, `expected an error to be thrown for "${args}"`)
}

function assert_node_type(node, type, prefix) {
    assert(node instanceof type, `[${prefix}] expected node type to be ${type}, got ${node.type}`)
}

function assert_range(range, begin_pos, end_pos, prefix) {
    assert(range instanceof Range, `[${prefix}] expected ${range} to be an instance of Range`)
    assert_eq(range.begin_pos, begin_pos, `[${prefix}] begin_pos`)
    assert_eq(range.end_pos, end_pos, `[${prefix}] end_pos`)
}

function assert_token(token, name, value, loc, prefix) {
    assert(token instanceof Token, `[${prefix}] expected ${token} to be an instance of Token`)
    assert_eq(token.name, name, `[${prefix}].name`)
    assert_eq(token.value, value, `[${prefix}].value`)
    assert_eq(token.loc.begin, loc.begin, `[${prefix}].loc.begin`)
    assert_eq(token.loc.end, loc.end, `[${prefix}].loc.end`)
}

function print_parse_result(parse_result) {
    console.log(inspect(result, { showHidden: false, depth: null }))
}

function bytes(str) {
    const bytes = unescape(encodeURIComponent(str)).split('').map(c => c.charCodeAt(0))
    return new Uint8Array(bytes)
}

class TestSuite {
    test_invalid_args() {
        assert_parse_throws(42)
        assert_parse_throws("foo", 10)
        assert_parse_throws(bytes("foo"), 10)
    }

    test_parse() {
        const result = parse(bytes("self.foo(123)"), {})
        assert(result !== null, "expected result to be non-null")

        const { ast } = result

        assert_node_type(ast, Send, 'send')
        const send = ast
        assert_range(send.dot_l, 4, 5, 'send.dot_l')
        assert_range(send.selector_l, 5, 8, 'send.selector_l')
        assert_range(send.begin_l, 8, 9, 'send.begin_l')
        assert_range(send.end_l, 12, 13, 'send.end_l')
        assert_eq(send.operator_l, null, '.operator_l')
        assert_range(send.expression_l, 0, 13, 'send.expression_l')

        assert_node_type(send.recv, Self_)
        const self = send.recv
        assert_range(self.expression_l, 0, 4, 'self.expression_l')

        assert_eq(send.method_name, "foo", 'send.method_name')

        assert_eq(send.args.length, 1, 'send.args.length')
        assert_node_type(send.args[0], Int)
        const arg = send.args[0]
        assert_eq(arg.value, "123", 'arg.value')
        assert_eq(arg.operator_l, null, 'arg.operator_l')
        assert_range(arg.expression_l, 9, 12, 'arg.expression_l')
    }

    test_tokens() {
        const result = parse(bytes("self.foo(123)"), { record_tokens: true })
        assert(result !== null, "expected result to be non-null")

        const { tokens } = result
        assert(tokens.length == 7)

        assert_token(tokens[0], "kSELF", "self", new Loc(0, 4), 'tokens[0]')
        assert_token(tokens[1], "tDOT", ".", new Loc(4, 5), 'tokens[1]')
        assert_token(tokens[2], "tIDENTIFIER", "foo", new Loc(5, 8), 'tokens[2]')
        assert_token(tokens[3], "tLPAREN2", "(", new Loc(8, 9), 'tokens[3]')
        assert_token(tokens[4], "tINTEGER", "123", new Loc(9, 12), 'tokens[4]')
        assert_token(tokens[5], "tRPAREN", ")", new Loc(12, 13), 'tokens[5]')
        assert_token(tokens[6], "EOF", "", new Loc(13, 13), 'tokens[6]')
    }

    test_diagnostics() {
        const result = parse(bytes("self.foo(123)"), { record_tokens: true })
        assert(result !== null, "expected result to be non-null")

        const { diagnostics } = result
        assert_eq(diagnostics.length, 0, diagnostics.length)
    }

    test_comments() {
        const result = parse(bytes("self.foo(123)"), { record_tokens: true })
        assert(result !== null, "expected result to be non-null")

        const { comments } = result
        assert_eq(comments.length, 0, 'comments.length')
    }

    test_magic_comments() {
        const result = parse(bytes("self.foo(123)"), { record_tokens: true })
        assert(result !== null, "expected result to be non-null")

        const { magic_comments } = result
        assert_eq(magic_comments.length, 0, 'magic_comments.length')
    }

    test_custom_decoder_ok() {
        const input = bytes("# encoding: us-ascii\n2 + 2");
        const custom_decoder_called_with = {}
        const custom_decoder = (encoding, input) => {
            custom_decoder_called_with.encoding = encoding;
            custom_decoder_called_with.input = input;
            const output = bytes("# encoding: us-ascii\n3 + 3")
            return output
        }
        const result = parse(input, { custom_decoder, record_tokens: true })
        assert(result !== null, "expected result to be non-null")

        const { tokens } = result
        assert_eq(tokens.length, 4, 'tokens.length')

        assert_token(tokens[0], "tINTEGER", "3", new Loc(21, 22), 'tokens[0]')
        assert_token(tokens[1], "tPLUS", "+", new Loc(23, 24), 'tokens[1]')
        assert_token(tokens[2], "tINTEGER", "3", new Loc(25, 26), 'tokens[2]')
        assert_token(tokens[3], "EOF", "", new Loc(26, 26), 'tokens[3]')

        assert_eq(custom_decoder_called_with.encoding, 'US-ASCII', 'encoding given to custom decoder')
        assert_array_eq(custom_decoder_called_with.input, input, 'input given to custom decoder')
    }

    test_custom_decoder_err() {
        const input = bytes("# encoding: us-ascii\n2 + 2");
        const custom_decoder = (encoding, input) => {
            return 'test error'
        }

        const result = parse(input, { custom_decoder })
        assert(result !== null, "expected result to be non-null")

        assert_eq(result.ast, null, 'ast');

        assert_eq(result.diagnostics.length, 1, 'diagnostics.length')
        const diagnostic = result.diagnostics[0]
        assert(diagnostic instanceof Diagnostic)
        assert_eq(diagnostic.level, 'error')
        assert_eq(diagnostic.message, 'encoding error: DecodingError("test error")')
        assert_range(diagnostic.range, 12, 20)
    }
}

const test_suite = new TestSuite()
test_suite.test_invalid_args()
test_suite.test_parse()
test_suite.test_tokens()
test_suite.test_diagnostics()
test_suite.test_comments()
test_suite.test_magic_comments()
test_suite.test_custom_decoder_ok()
test_suite.test_custom_decoder_err()

console.log("All tests passed")
