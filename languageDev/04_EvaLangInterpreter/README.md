# Eva Language Interpreter

Eva is a simple programming language interpreter written in JavaScript, designed to showcase key features like variable declaration, loops, functions, and object-oriented programming. It aims to provide a learning tool for understanding interpreters and language design while allowing users to interact with a custom syntax.

## Overview

Eva is a lightweight programming language built around expressions. It supports basic control flow structures (such as `if`, `for`, `while`), functions (both anonymous and named), and object-oriented features like classes and inheritance. This project is an interpreter, which means that it reads and executes code written in Eva directly from the command line or as part of another program.

The Eva interpreter is built in JavaScript and is fully extensible with user-defined functions, modules, and built-in methods for basic arithmetic, comparisons, and more.

## Features

- **Variable Declaration & Assignment:** Using `var` and `set` for defining and modifying variables.
- **Control Flow:** Implements common flow control structures like `if`, `for`, `while`, and `switch`.
- **Functions:** Supports both named (`def`) and anonymous (`lambda`) functions.
- **Classes & Objects:** Object-oriented programming with class definitions, inheritance, and object instantiation.
- **Modules & Imports:** Allows for modular code and the ability to import external `.eva` files.
- **Arithmetic & Comparison Operations:** Built-in support for addition, subtraction, multiplication, division, and logical comparison operators.
- **Customizable Environment:** Easily extendable environment to add custom behavior or override built-in functions.

### Running the Interpreter

To run the Eva interpreter interactively, use the following command in your terminal:

```bash
./bin/eva -f ../filename.eva
```

### Writing Eva Code

Eva code follows a Lisp-like syntax with expressions written in prefix form. Here is an example of a simple program in Eva:

```eva
(begin
    (var x 10)
    (var y 5)
    (set x (+ x y))
    (print x)  ; Output: 15
)
```

### Eva Syntax Overview

- `begin`: A block that wraps multiple expressions.
- `var`: Declare a variable.
- `set`: Assign a new value to an existing variable.
- `if`: Conditional statement.
- `while`: Loop statement.
- `def`: Define a function.
- `lambda`: Define an anonymous function.
- `class`: Define a class.
- `new`: Instantiate a class.
- `prop`: Access a property in an object.

## Resources
- [Dmitry Soshnikov](http://dmitrysoshnikov.com/).