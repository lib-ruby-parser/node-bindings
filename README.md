# Node.js bindings for `lib-ruby-parser`

It's based on [C++ bindings](https://github.com/lib-ruby-parser/cpp-bindings) and `node-addon-api`.

Each node has its own JavaScript class, so
1. it's possible to dinstinguish them by checking `instanceof`
2. they can be extended in pure JavaScript

Here's how it looks like:

```js
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
      value: '2',
      loc: Loc { begin: 0, end: 1 }
    },
    Token { name: 'tPLUS', value: '+', loc: Loc { begin: 2, end: 3 } },
    Token {
      name: 'tINTEGER',
      value: '3',
      loc: Loc { begin: 4, end: 5 }
    },
    Token { name: 'EOF', value: '', loc: Loc { begin: 5, end: 5 } }
  ],
  diagnostics: [],
  comments: [],
  magic_comments: [],
  input: '2 + 3'
}
```

## API

tldr; all classes mirror Rust implementation.

TypeScript definition:

```ts
interface Loc { begin: number, end: number }
interface Range { begin_pos: number, end_pos: number }

interface Token {
    name: string,
    value: string,
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
