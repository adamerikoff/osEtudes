// Token specification for the tokenizer, containing regular expressions and corresponding token types.
// If a token type is `null`, it will be ignored (e.g., whitespace, comments).
const Spec = [
    // Whitespace and comments (ignored tokens)
    [/^\s+/, null],                // Match whitespace (ignored)
    [/^\/\/.*/, null],             // Match single-line comments (ignored)
    [/^\/\*[\s\S]*?\*\//, null],   // Match multi-line comments (ignored)

    // Symbols (punctuation and delimiters)
    [/^;/, ";"],                   // Semicolon
    [/^\{/, "{"],                  // Opening brace
    [/^\}/, "}"],                  // Closing brace
    [/^\(/, "("],                  // Opening parenthesis
    [/^\)/, ")"],                  // Closing parenthesis
    [/^,/, ","],                   // Comma
    [/^\./, "."],                   // Dot (.)
    [/^\[/, "["],                   // Opening square bracket
    [/^\]/, "]"],                   // Closing square bracket

    // Keywords (reserved words)
    [/^\blet\b/, "let"],           // `let` keyword for variable declaration
    [/^\bif\b/, "if"],             // `if` keyword for conditional statements
    [/^\belse\b/, "else"],         // `else` keyword for alternative conditional branch
    [/^\bfalse\b/, "false"],       // Boolean `false`
    [/^\btrue\b/, "true"],         // Boolean `true`
    [/^\bnull\b/, "null"],         // Null literal
    [/^\bwhile\b/, "while"],       // `while` keyword for loop
    [/^\bdo\b/, "do"],             // `do` keyword for do-while loop
    [/^\bfor\b/, "for"],           // `for` keyword for loop
    [/^\bdef\b/, "def"],           // `def` keyword for function definition
    [/^\breturn\b/, "return"],     // `return` keyword for returning values from functions
    [/^\bclass\b/, "class"],       // `class` keyword for defining classes
    [/^\bextends\b/, "extends"],   // `extends` keyword for inheritance in classes
    [/^\bsuper\b/, "super"],       // `super` keyword for accessing parent class methods/properties
    [/^\bnew\b/, "new"],           // `new` keyword for creating new instances
    [/^\bthis\b/, "this"],         // `this` keyword for referring to the current instance

    // Literals (data values)
    [/^\d+/, "NUMBER"],            // Numeric literal (integer values)
    [/^"[^"]*"/, "STRING"],        // String literal (double-quoted)

    // Identifiers (names for variables, functions, etc.)
    [/^\w+/, "IDENTIFIER"],        // Identifier (variable, function, class, etc.)

    // Operators (used for operations in expressions)
    [/^[=!]=/, "EQUALITY_OPERATOR"],       // Equality operators (==, !=)
    [/^=/, "SIMPLE_ASSIGN"],               // Simple assignment operator (=)
    [/^[\*\/\+\-]=/, "COMPLEX_ASSIGN"],    // Complex assignment operators (*=, /=, +=, -=)
    [/^[+\-]/, "ADDITIVE_OPERATOR"],       // Additive operators (+, -)
    [/^[*\/]/, "MULTIPLICATIVE_OPERATOR"], // Multiplicative operators (*, /)
    [/^[><]=?/, "RELATIONAL_OPERATOR"],    // Relational operators (<, <=, >, >=)
    [/^&&/, "LOGICAL_AND"],                // Logical AND operator (&&)
    [/^\|\|/, "LOGICAL_OR"],               // Logical OR operator (||)
    [/^!/, "LOGICAL_NOT"],                 // Logical NOT operator (!)
];

module.exports = { 
    Spec,
};
