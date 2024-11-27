#ifndef OPDCODE_H
#define OPDCODE_H

#include "../logger/Logger.h"

// Each opcode represents a specific instruction for the virtual machine

// Halts the program execution
#define OP_HALT 0x00

// Pushes a constant value onto the stack
#define OP_CONST 0x01

// Arithmetic operations
#define OP_ADD 0x02
#define OP_SUB 0x03
#define OP_MUL 0x04
#define OP_DIV 0x05

// Comparison operation
#define OP_COMPARE 0x06

// Control flow instructions
#define OP_JMP_IF_FALSE 0x07 // Jump if the top of the stack is false
#define OP_JUMP 0x08 // Unconditional jump

// Global variable access
#define OP_GET_GLOBAL 0x09
#define OP_SET_GLOBAL 0x10

// Stack operations
#define OP_POP 0x11 // Pop the top value from the stack

// Local variable access
#define OP_GET_LOCAL 0x12
#define OP_SET_LOCAL 0x13

// Scope management
#define OP_SCOPE_EXIT 0x14 // Exit the current scope

// Function calls and returns
#define OP_CALL 0x15
#define OP_RETURN 0x16

// Cell operations (for closures)
#define OP_GET_CELL 0x17
#define OP_SET_CELL 0x18
#define OP_LOAD_CELL 0x19

// Function creation and object instantiation
#define OP_MAKE_FUNCTION 0x20
#define OP_NEW 0x21

// Property access and assignment
#define OP_GET_PROP 0x22
#define OP_SET_PROP 0x23

// Macro to create a case statement for each opcode in the `opcodeToString` function
#define OP_STR(op) \
    case OP_##op: \
        return #op

// Function to convert an opcode to its string representation
// This function is used for debugging and logging purposes
inline std::string opcodeToString(uint8_t opcode) {
    switch (opcode) {
        OP_STR(HALT);
        OP_STR(CONST);
        OP_STR(ADD);
        OP_STR(SUB);
        OP_STR(MUL);
        OP_STR(DIV);
        OP_STR(COMPARE);
        OP_STR(JMP_IF_FALSE);
        OP_STR(JUMP);
        OP_STR(GET_GLOBAL);
        OP_STR(SET_GLOBAL);
        OP_STR(POP);
        OP_STR(GET_LOCAL);
        OP_STR(SET_LOCAL);
        OP_STR(SCOPE_EXIT);
        OP_STR(CALL);
        OP_STR(RETURN);
        OP_STR(GET_CELL);
        OP_STR(SET_CELL);
        OP_STR(LOAD_CELL);
        OP_STR(MAKE_FUNCTION);
        OP_STR(NEW);
        OP_STR(GET_PROP);
        OP_STR(SET_PROP);
        default:
            // Handle unknown opcodes gracefully
            DIE << "opcodeToString: Unknown opcode: " << (int)opcode;
    }

    return "Unknown";
}

#endif