#!/usr/bin/env node

let path_to_require = process.argv[2]

if (!path_to_require) {
    console.log('no argument specified, using debug build')
    path_to_require = './build/Debug/ruby_parser.node'
}

console.log(`requiring ${path_to_require}`)
const { parse, Loc, Send, Self_, Int, Token, Diagnostic, bytes_to_utf8_lossy } = require(path_to_require)
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

function loc(begin, end) {
    let loc = new Loc()
    loc.begin = begin
    loc.end = end
    return loc
}

function assert_loc(loc, begin, end, prefix) {
    assert(loc instanceof Loc, `[${prefix}] expected ${loc} to be an instance of Loc`)
    assert_eq(loc.begin, begin, `[${prefix}] begin`)
    assert_eq(loc.end, end, `[${prefix}] end`)
}

function assert_token(token, name, value, loc, prefix) {
    assert(token instanceof Token, `[${prefix}] expected ${token} to be an instance of Token`)
    assert_eq(token.name, name, `[${prefix}].name`)
    assert_eq(bytes_to_utf8_lossy(token.value), value, `[${prefix}].value`)
    assert_eq(token.loc.begin, loc.begin, `[${prefix}].loc.begin`)
    assert_eq(token.loc.end, loc.end, `[${prefix}].loc.end`)
}

function print_parser_result(parser_result) {
    console.log(inspect(parser_result, { showHidden: false, depth: null }))
}

function str_to_bytes(str) {
    const bytes = unescape(encodeURIComponent(str)).split('').map(c => c.charCodeAt(0))
    return new Uint8Array(bytes)
}

function bytes_to_str(bytes) {
    return decodeURIComponent(Array.from(bytes).map(c => String.fromCharCode(c)).join(''))
}

class TestSuite {
    test_invalid_args() {
        assert_parse_throws(42)
        assert_parse_throws("foo", 10)
        assert_parse_throws(str_to_bytes("foo"), 10)
    }

    test_parse() {
        const result = parse(str_to_bytes("self.foo(123)"), {})
        assert(result !== null, "expected result to be non-null")

        const { ast } = result

        assert_node_type(ast, Send, 'send')
        const send = ast
        assert_loc(send.dot_l, 4, 5, 'send.dot_l')
        assert_loc(send.selector_l, 5, 8, 'send.selector_l')
        assert_loc(send.begin_l, 8, 9, 'send.begin_l')
        assert_loc(send.end_l, 12, 13, 'send.end_l')
        assert_eq(send.operator_l, null, '.operator_l')
        assert_loc(send.expression_l, 0, 13, 'send.expression_l')

        assert_node_type(send.recv, Self_)
        const self = send.recv
        assert_loc(self.expression_l, 0, 4, 'self.expression_l')

        assert_eq(send.method_name, "foo", 'send.method_name')

        assert_eq(send.args.length, 1, 'send.args.length')
        assert_node_type(send.args[0], Int)
        const arg = send.args[0]
        assert_eq(arg.value, "123", 'arg.value')
        assert_eq(arg.operator_l, null, 'arg.operator_l')
        assert_loc(arg.expression_l, 9, 12, 'arg.expression_l')
    }

    test_tokens() {
        const result = parse(str_to_bytes("self.foo(123)"), { record_tokens: true })
        assert(result !== null, "expected result to be non-null")

        const { tokens } = result
        assert(tokens.length == 7)

        assert_token(tokens[0], "kSELF", "self", loc(0, 4), 'tokens[0]')
        assert_token(tokens[1], "tDOT", ".", loc(4, 5), 'tokens[1]')
        assert_token(tokens[2], "tIDENTIFIER", "foo", loc(5, 8), 'tokens[2]')
        assert_token(tokens[3], "tLPAREN2", "(", loc(8, 9), 'tokens[3]')
        assert_token(tokens[4], "tINTEGER", "123", loc(9, 12), 'tokens[4]')
        assert_token(tokens[5], "tRPAREN", ")", loc(12, 13), 'tokens[5]')
        assert_token(tokens[6], "EOF", "", loc(13, 13), 'tokens[6]')
    }

    test_diagnostics() {
        const result = parse(str_to_bytes("self.foo(123)"), { record_tokens: true })
        assert(result !== null, "expected result to be non-null")

        const { diagnostics } = result
        assert_eq(diagnostics.length, 0, diagnostics.length)
    }

    test_comments() {
        const result = parse(str_to_bytes("self.foo(123)"), { record_tokens: true })
        assert(result !== null, "expected result to be non-null")

        const { comments } = result
        assert_eq(comments.length, 0, 'comments.length')
    }

    test_magic_comments() {
        const result = parse(str_to_bytes("self.foo(123)"), { record_tokens: true })
        assert(result !== null, "expected result to be non-null")

        const { magic_comments } = result
        assert_eq(magic_comments.length, 0, 'magic_comments.length')
    }

    test_custom_decoder_ok() {
        const input = str_to_bytes("# encoding: us-ascii\n2 + 2");
        const custom_decoder_called_with = {}
        const custom_decoder = (encoding, input) => {
            custom_decoder_called_with.encoding = encoding;
            custom_decoder_called_with.input = input;
            const output = str_to_bytes("# encoding: us-ascii\n3 + 3")
            return output
        }
        const result = parse(input, { custom_decoder, record_tokens: true })
        assert(result !== null, "expected result to be non-null")

        const { tokens } = result
        assert_eq(tokens.length, 4, 'tokens.length')

        assert_token(tokens[0], "tINTEGER", "3", loc(21, 22), 'tokens[0]')
        assert_token(tokens[1], "tPLUS", "+", loc(23, 24), 'tokens[1]')
        assert_token(tokens[2], "tINTEGER", "3", loc(25, 26), 'tokens[2]')
        assert_token(tokens[3], "EOF", "", loc(26, 26), 'tokens[3]')

        assert_eq(custom_decoder_called_with.encoding, 'US-ASCII', 'encoding given to custom decoder')
        assert_array_eq(custom_decoder_called_with.input, input, 'input given to custom decoder')
    }

    test_custom_decoder_err() {
        const input = str_to_bytes("# encoding: us-ascii\n2 + 2");
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
        assert_loc(diagnostic.loc, 12, 20)
    }

    test_loc_source() {
        const input = parse(str_to_bytes("foo.bar(42)"), {}).input;

        let source = loc(1, 8).source(input);
        assert_eq(bytes_to_str(source), "oo.bar(");
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
test_suite.test_loc_source();

console.log("All tests passed")
