import { parse, nodes, Loc, Token, tokens, Diagnostic, messages, Comment, MagicComment, DecodedInput, SourceLine } from 'lib-ruby-parser'

const NO_DECODER = (_encoding: string, _input: Uint8Array) => {
    throw new Error("Decoder is not configured")
}

it.concurrent('handles invalid arguments', () => {
    expect(() => {
        parse(42 as any as string, '(eval)', NO_DECODER)
    }).toThrowError('`input` argument must be a String')

    expect(() => {
        parse('42', 42 as any as string, NO_DECODER)
    }).toThrowError('`buffer_name` argument must be a String')

    expect(() => {
        parse('42', '42', 42 as any as (e: string, i: Uint8Array) => Uint8Array)
    }).toThrowError('`decode` argument must be a function')
})

it.concurrent('parses AST', () => {
    expect(
        parse("2 + 3", "(eval)", NO_DECODER).ast
    ).toEqual(
        new nodes.Send(
            new nodes.Int(
                "2",
                null,
                new Loc(0, 1)
            ),
            "+",
            [
                new nodes.Int(
                    "3",
                    null,
                    new Loc(4, 5),
                )
            ],
            null,
            new Loc(2, 3),
            null,
            null,
            null,
            new Loc(0, 5)
        )
    );
})

it.concurrent('parses tokens', () => {
    expect(
        parse('2 + 3', '(eval)', NO_DECODER).tokens
    ).toEqual([
        new Token(
            tokens.name_to_id["tINTEGER"]!,
            new TextEncoder().encode('2'),
            new Loc(0, 1),
            1,
            2
        ),
        new Token(
            tokens.name_to_id["tPLUS"]!,
            new TextEncoder().encode('+'),
            new Loc(2, 3),
            2,
            1
        ),
        new Token(
            tokens.name_to_id["tINTEGER"]!,
            new TextEncoder().encode('3'),
            new Loc(4, 5),
            1,
            2
        ),
        new Token(
            tokens.name_to_id["END_OF_INPUT"]!,
            new TextEncoder().encode(''),
            new Loc(5, 5),
            2,
            2
        )
    ])
})

it.concurrent('returns diagnostics', () => {
    expect(
        parse('def foo; A = 1; end', '(eval)', NO_DECODER).diagnostics,
    ).toEqual([
        new Diagnostic(
            "error",
            new messages.DynamicConstantAssignment(),
            new Loc(9, 10)
        )
    ])
})

it.concurrent('returns comments', () => {
    expect(
        parse('foo # bar\n=begin\n=end', '(eval)', NO_DECODER).comments
    ).toEqual([
        new Comment(
            new Loc(4, 10),
            "inline"
        ),
        new Comment(
            new Loc(10, 21),
            "document"
        )
    ])
})

it.concurrent('returns magic comments', () => {
    expect(
        parse('# frozen_string_literal: true\nfoo', '(eval)', NO_DECODER).magic_comments
    ).toEqual([
        new MagicComment(
            "frozen_string_literal",
            new Loc(2, 23),
            new Loc(25, 29)
        )
    ])
})

it.concurrent('returns parsed input', () => {
    expect(
        parse('foo\nbar', '(eval)', NO_DECODER).input
    ).toEqual(
        new DecodedInput(
            '(eval)',
            [
                new SourceLine(0, 4, false),
                new SourceLine(4, 7, true),
            ],
            new TextEncoder().encode('foo\nbar')
        )
    )
})

it.concurrent('accepts custom decoder', () => {
    const initialSrc = '# encoding: unknown\ndecode me';
    const decodedSrc = '# encoding: unknown\ndecoded output';
    let calledWithEncoding = "";
    let calledWithInput = new Uint8Array();

    function customDecoder(encoding: string, input: Uint8Array): Uint8Array {
        calledWithEncoding = encoding;
        calledWithInput = input;

        return new TextEncoder().encode(decodedSrc);
    }

    expect(
        parse(initialSrc, '(eval)', customDecoder).input.bytes
    ).toEqual(
        new TextEncoder().encode(decodedSrc)
    );
    expect(calledWithEncoding).toEqual('unknown');
    expect(
        new TextDecoder().decode(calledWithInput)
    ).toEqual(initialSrc);
})
