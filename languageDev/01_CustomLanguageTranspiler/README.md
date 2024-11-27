# Custom Language Transpiler

This project implements a simple transpiler that takes code written in a custom language, tokenizes and parses it, and then generates JavaScript code. This basic transpiler includes a tokenizer, parser, and code generator to enable source-to-source transformation from the custom language to JavaScript.

## Project Structure

The main files and classes in the project are as follows:

- **Tokenizer**: Reads the source code and breaks it down into individual tokens. Each token represents a basic language construct such as keywords, identifiers, parentheses, and integers.
- **Parser**: Takes the list of tokens and organizes them into a syntax tree, identifying the structure and relationships within the code. The parser can recognize function definitions, function calls, and integer expressions.
- **Generator**: Translates the syntax tree into equivalent JavaScript code. This generated code can then be executed in a JavaScript environment.

## Code Components

### 1. Tokenizer

The `Tokenizer` class converts source code into tokens. It identifies specific constructs based on patterns defined in `TOKEN_TYPES`:
  - `def`: Function definition keyword.
  - `end`: Keyword to indicate the end of a function.
  - `identifier`: Variable or function names (alphabetic characters).
  - `integer`: Numeric constants.
  - `oparen` and `cparen`: Parentheses `(` and `)` for function calls and argument lists.
  - `comma`: Separator for function arguments.

### 2. Parser

The `Parser` class takes a list of tokens and organizes them into a syntax tree. Key methods in `Parser` include:

- `parse_def`: Parses a function definition.
- `parse_arg_names`: Parses function argument names.
- `parse_expr`: Determines if an expression is an integer or function call.
- `parse_int` and `parse_call`: Parse integer values and function calls.
- `parse_var_ref`: Parses variable references within expressions.

The parser builds and returns an Abstract Syntax Tree (AST) made up of custom nodes (`DefNode`, `CallNode`, `IntegerNode`, and `VarRefNode`).

### 3. Generator

The `Generator` class takes the AST and converts it into JavaScript code. It has support for:

- Generating JavaScript function definitions from `DefNode` AST nodes.
- Generating JavaScript function calls from `CallNode` AST nodes.
- Translating `IntegerNode` and `VarRefNode` to JavaScript-compatible representations.

### Node Definitions

The project includes several node types representing different elements of the AST:
- **DefNode**: Represents a function definition with a name, argument list, and body.
- **CallNode**: Represents a function call with a name and list of argument expressions.
- **IntegerNode**: Represents integer values.
- **VarRefNode**: Represents variable references.

## Example Usage

To run the transpiler, follow these steps:

1. **Set up a source file**: Create a file, for example, `test.src`, containing code in the custom language:
   ```plaintext
   def multiply(x, y)
     x * y
   end
   ```

2. **Run the transpiler**: Execute the main program code which uses `Tokenizer`, `Parser`, and `Generator` to convert `test.src` to JavaScript:
   ```ruby
   require "./Tokenizer.rb"
   require "./Parser.rb"
   require "./Generator.rb"

   RUNTIME = "function multiply(x,y) { return x * y };"
   LOG = "console.log(multiply(2,10));"

   tokens = Tokenizer.new(File.read("test.src")).tokenize
   tree = Parser.new(tokens).parse
   generated = Generator.new.generate(tree)

   puts [RUNTIME, generated, LOG].join("\n")
   ```
   The output should display the JavaScript code equivalent of the custom source code in `test.src`.

3. **Run the output in JavaScript**: Copy the generated JavaScript code and run it in a JavaScript environment like Node.js or a browser console.

## Dependencies

This project requires Ruby to run.

## Resources

[Destroy All Software](https://www.destroyallsoftware.com).
