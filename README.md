# Node.js bindings for `lib-ruby-parser`

It's based on [C++ bindings](https://github.com/lib-ruby-parser/cpp-bindings) and `node-addon-api`.

Each node has its own JavaScript class, so
1. it's possible to dinstinguish them by checking `instanceof`
2. they can be extended in pure JavaScript

Basic usage:

```js
const inspect = require('util').inspect
const { parse, Send } = require('lib-ruby-parser');

function print_parse_result(parser_result) {
    console.log(inspect(parser_result, { showHidden: false, depth: null }))
}

// This function must be defined by you.
// It takes a string and return an array of bytes.
// The following code is just an example:
function bytes(str) {
    const bytes = unescape(encodeURIComponent(str)).split('').map(c => c.charCodeAt(0))
    return new Uint8Array(bytes)
}

const input = bytes("2 + 3 # x");
const options = { record_tokens: true };
const result = parse(input, options);
print_parser_result(result);

// prints:

ParserResult {
  ast: Send {
    recv: Int {
      value: '2',
      operator_l: null,
      expression_l: Range { begin_pos: 0, end_pos: 1 }
    },
    method_name: '+',
    args: [
      Int {
        value: '3',
        operator_l: null,
        expression_l: Range { begin_pos: 4, end_pos: 5 }
      }
    ],
    dot_l: null,
    selector_l: Range { begin_pos: 2, end_pos: 3 },
    begin_l: null,
    end_l: null,
    operator_l: null,
    expression_l: Range { begin_pos: 0, end_pos: 5 }
  },
  tokens: [
    Token {
      name: 'tINTEGER',
      value: Uint8Array(1) [ 50 ],
      loc: Loc { begin: 0, end: 1 }
    },
    Token {
      name: 'tPLUS',
      value: Uint8Array(1) [ 43 ],
      loc: Loc { begin: 2, end: 3 }
    },
    Token {
      name: 'tINTEGER',
      value: Uint8Array(1) [ 51 ],
      loc: Loc { begin: 4, end: 5 }
    },
    Token {
      name: 'tNL',
      value: Uint8Array(1) [ 10 ],
      loc: Loc { begin: 8, end: 9 }
    },
    Token {
      name: 'EOF',
      value: Uint8Array(0) [],
      loc: Loc { begin: 8, end: 8 }
    }
  ],
  diagnostics: [],
  comments: [
    Comment {
      kind: 'inline',
      location: Range { begin_pos: 6, end_pos: 9 }
    }
  ],
  magic_comments: [],
  input: Uint8Array(9) [
    50, 32, 43,  32, 51,
    32, 35, 32, 120
  ]
}
```

## API

tldr; all classes mirror Rust implementation.

Rough TypeScript definition:

```ts
interface Loc { begin: number, end: number }
interface Range { begin_pos: number, end_pos: number }

interface Token {
    name: string,
    value: Uint8Array,
    loc: Loc
}

interface Args {
    // mirrors https://docs.rs/lib-ruby-parser/0.7.0/lib_ruby_parser/nodes/struct.Args.html
    args: Array<Node>,
    expression_l: Range,
    begin_l: Range | null,
    end_l: Range | null
}

interface Class {
    // mirrors https://docs.rs/lib-ruby-parser/0.7.0/lib_ruby_parser/nodes/struct.Class.html
    name: Node,
    superclass: Node | null,
    body: Node | null,
    keyword_l: Range,
    operator_l: Range | null,
    end_l: Range,
    expression_l: Range
}

// other ~100 nodes

type Node = Args | Class | ... /* other nodes */;

function parse(code: String): ParserResult
```

`String` and `Symbol` nodes are slightly exceptional as they contain `StringValue` Rust structure that is a `Uint8Array` here.

This structure can be converted into JS `String` by using `bytes_to_utf8_lossy` function (keep in mind that it replaces unknown chars with a special `Unicode Replacement Character U+FFFD`, if you want some other strategy you are free to define your own converting function):

```js
const { parse, bytes_to_utf8_lossy } = require(path_to_require)

const result = parse(bytes('"a\\xFFb"'), { record_tokens: true });
console.log(result.ast)
console.log(bytes_to_utf8_lossy(result.ast.value))

// prints

Str {
  value: Uint8Array(3) [ 97, 255, 98 ], // "a" = 97, "\xFF" = 255, "b" = 98
  begin_l: Range { begin_pos: 0, end_pos: 1 },
  end_l: Range { begin_pos: 7, end_pos: 8 },
  expression_l: Range { begin_pos: 0, end_pos: 8 }
}

a�b
```

## Encodings

If you want to support encodings other than UTF-8/ASCII-8BIT/BINARY you need a custom decoder:

```js
const custom_decoder = (encoding: String, input: Uint8Array) => {
  // Do some **real** decoding into UTF-8 here
  //
  // Here for simplicity we convert all "2" into "3"
  //
  assert(encoding === "US-ASCII");
  assert(input === bytes("# encoding: us-ascii\n2 + 2"));

  return bytes("# encoding: us-ascii\n3 + 3");
}

const result = parse(
  bytes("# encoding: us-ascii\n2 + 2"),
  { custom_decoder }
);
assert(result.ast.recv.value === "3");
```

## Platform support

Currently NPM packages include pre-compiled `.node` files for Mac and Linux.

If you need Windows support first go to [C++ bindings](https://github.com/lib-ruby-parser/cpp-bindings) and create an issue there. This repo is just a wrapper around its builds.
