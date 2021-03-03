import { parse, Loc, Send, Self_, Int, Token, Diagnostic, bytes_to_utf8_lossy, UnexpectedToken, ParserOptions, EncodingError } from 'lib-ruby-parser';

const assert = require('assert').strict
const inspect = require('util').inspect

function assert_eq<T>(actual: T, expected: T, prefix: string) {
    assert(actual === expected, `[${prefix}] assertion failed: expected ${expected}, got ${actual}`)
}

function assert_array_eq(actual, expected, prefix) {
    assert_eq(actual.length, expected.length, `${prefix}.length`)
    for (let i = 0; i < actual.length; i++) {
        assert_eq(actual[i], expected[i], `${prefix}[${i}]`)
    }
}

function assert_parse_throws(input: Uint8Array, options: ParserOptions) {
    let thrown = false
    try {
        parse(input, options)
    } catch (e) {
        thrown = true
    }

    assert(thrown, `expected an error to be thrown for "${input}, ${options}"`)
}

function assert_loc(actual: any, expected: Loc, prefix) {
    if (!(actual instanceof Loc)) {
        throw `[${prefix}] expected ${actual} to be an instance of Loc`
    }
    assert_eq(actual.begin, expected.begin, `[${prefix}] begin`)
    assert_eq(actual.end, expected.end, `[${prefix}] end`)
}

function assert_token(token: any, name: string, value: string, loc: Loc, prefix: string): token is Token {
    if (!(token instanceof Token)) {
        throw `[${prefix}] expected ${token} to be an instance of Token`
    }
    assert_eq(token.name, name, `[${prefix}].name`)
    assert_eq(bytes_to_utf8_lossy(token.value), value, `[${prefix}].value`)
    assert_loc(token.loc, loc, `[${prefix}].loc`)
    return true;
}

function assert_diagnostic(diagnostic: any, loc: Loc, rendered: string, prefix: string) {
    if (!(diagnostic instanceof Diagnostic)) {
        throw `[${prefix}] expected diagnostic to be Diagnostic, got ${diagnostic}`
    }
    assert_loc(diagnostic.loc, loc, `[${prefix}].loc`)
    assert_eq(diagnostic.rendered, rendered, `[${prefix}].rendered`)
}

function print_parser_result(parser_result) {
    console.log(inspect(parser_result, { showHidden: false, depth: null }))
}

function str_to_bytes(str: string) {
    const bytes = unescape(encodeURIComponent(str)).split('').map(c => c.charCodeAt(0))
    return new Uint8Array(bytes)
}

function bytes_to_str(bytes: Uint8Array) {
    return decodeURIComponent(Array.from(bytes).map(c => String.fromCharCode(c)).join(''))
}

class TestSuite {
    test_invalid_args() {
        assert_parse_throws(42 as any, 56 as any)
        assert_parse_throws("foo" as any, 10)
        assert_parse_throws(str_to_bytes("foo"), 10)
    }

    test_parse() {
        const result = parse(str_to_bytes("self.foo(123)"), {})
        assert(result !== null, "expected result to be non-null")

        const { ast } = result

        if (!(ast instanceof Send)) {
            throw `wrong node type (expected Send, got ${ast})`
        }
        const send = ast
        assert_loc(send.dot_l, new Loc(4, 5), 'send.dot_l')
        assert_loc(send.selector_l, new Loc(5, 8), 'send.selector_l')
        assert_loc(send.begin_l, new Loc(8, 9), 'send.begin_l')
        assert_loc(send.end_l, new Loc(12, 13), 'send.end_l')
        assert_eq(send.operator_l, null, '.operator_l')
        assert_loc(send.expression_l, new Loc(0, 13), 'send.expression_l')

        if (!(send.recv instanceof Self_)) {
            throw `wrong node type (expected Self, got ${send.recv})`
        }
        const self = send.recv
        assert_loc(self.expression_l, new Loc(0, 4), 'self.expression_l')

        assert_eq(send.method_name, "foo", 'send.method_name')

        assert_eq(send.args.length, 1, 'send.args.length')
        if (!(send.args[0] instanceof Int)) {
            throw `wrong node type (expected Self, got ${send.args[0]})`
        }
        const arg = send.args[0]
        assert_eq(arg.value, "123", 'arg.value')
        assert_eq(arg.operator_l, null, 'arg.operator_l')
        assert_loc(arg.expression_l, new Loc(9, 12), 'arg.expression_l')
    }

    test_tokens() {
        const result = parse(str_to_bytes("self.foo(123)"), { record_tokens: true })
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
        const result = parse(str_to_bytes("(123]"), { record_tokens: true })
        assert(result !== null, "expected result to be non-null")

        const { diagnostics } = result
        assert_eq(diagnostics.length, 1, 'diagnostics.length')
        const diagnostic = diagnostics[0];

        assert_diagnostic(diagnostic, new Loc(4, 5), 'unexpected tRBRACK', 'diagnosticss[0]')
        if (!(diagnostic.message instanceof UnexpectedToken)) { throw `expected diagnostic.message to be UnexpectedToken, got ${diagnostic.message}` }
        assert_eq(diagnostic.message.token_name, 'tRBRACK', 'diagnostic.message.token_name')
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
        const custom_decoder_called_with: { encoding?: string, input?: Uint8Array } = {}
        const custom_decoder = (encoding: string, input: Uint8Array) => {
            custom_decoder_called_with.encoding = encoding;
            custom_decoder_called_with.input = input;
            const output = str_to_bytes("# encoding: us-ascii\n3 + 3")
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
        const input = str_to_bytes("# encoding: us-ascii\n2 + 2");
        const custom_decoder = (encoding, input) => {
            return 'test error'
        }

        const result = parse(input, { custom_decoder })
        assert(result !== null, "expected result to be non-null")

        assert_eq(result.ast, null, 'ast');

        assert_eq(result.diagnostics.length, 1, 'diagnostics.length')
        const diagnostic = result.diagnostics[0]

        assert_diagnostic(diagnostic, new Loc(12, 20), 'encoding error: DecodingError("test error")', 'diagnosticss[0]')
        if (!(diagnostic.message instanceof EncodingError)) { throw `expected diagnostic.message to be UnexpectedToken, got ${diagnostic.message}` }
        assert_eq(diagnostic.message.error, 'DecodingError("test error")', 'diagnostic.message.error')
    }

    test_loc_source() {
        const input = parse(str_to_bytes("foo.bar(42)"), {}).input;

        let source = new Loc(1, 8).source(input);
        assert_eq(bytes_to_str(source), "oo.bar(", "source of [1,8] loc");
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
