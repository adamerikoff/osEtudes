# EvaLangVM

EvaLangVM is a simple virtual machine (VM) designed to execute programs written in **EvaLang**. The project includes a compiler, a virtual machine, and a disassembler for EvaLang bytecode, as well as a garbage collector to manage memory.

### Key Components

- **run.sh**: A shell script that builds and runs the project. This automates the process of setting up the environment and starting the application.
  
- **bytecode/OpCode.h**: Contains the opcodes (instructions) for the virtual machine. This defines the set of operations that the VM can perform.

- **compiler/**: This directory contains all the components necessary to compile EvaLang source code into bytecode. The `EvaCompiler.cpp` file implements the logic for translating EvaLang code to bytecode, while `Scope.cpp` manages the variable scope during compilation.

- **disassembler/**: This directory includes the disassembler, which converts bytecode back into human-readable source code. `EvaDisassembler.cpp` performs this disassembly.

- **includes.h**: This file includes common headers and utilities used across multiple parts of the project, such as standard libraries and shared definitions.

- **logger/**: Contains the logger utility used for debugging and logging runtime information. It provides an easy way to track errors or runtime events.

- **main.cpp**: The entry point for the program, where the VM is initialized and the program is executed. This file sets up the virtual machine and loads the program from a source file.

- **parser/**: Responsible for parsing the source code of EvaLang. The `EvaParser.h` header defines the parsing logic, while `grammar.bnf` defines the syntax of the language.

- **vm/**: This folder contains the core implementation of the virtual machine. It includes files for managing VM state, memory, garbage collection, and value representation.

## Building the Project

To build the EvaLangVM project, follow these steps:

1. **Clone the repository**:
   ```bash
   ./run.sh
   ```

5. **Run the project**:
   Once the build is complete, you can run the VM with the following command:
   ```bash
   ./build/EvaLangVM ./example.code
   ```

## Example Code

Here’s a simple example of a program written in EvaLang:

```eva
// Simple EvaLang Program
(+ 2 2)
```

## Resources
- [Dmitry Soshnikov](http://dmitrysoshnikov.com/).

