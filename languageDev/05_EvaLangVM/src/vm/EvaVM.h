#ifndef EVAVM_H
#define EVAVM_H





#include "../includes.h"
#include "../bytecode/OpCode.h"
#include "../compiler/EvaCompiler.h"
#include "../logger/Logger.h"
#include "../parser/EvaParser.h"
#include "EvaValue.h"
#include "Global.h"
#include "EvaCollector.h"

using syntax::EvaParser;

#define READ_BYTE() *ip++
#define STACK_LIMIT 512
#define GET_CONST() fn->co->constants[READ_BYTE()]
#define READ_SHORT() (ip += 2, (uint16_t)((ip[-2] << 8) | ip[-1]))
#define TO_ADDRESS(index) &fn->co->code[index]

#define MEM(allocator, ...) (maybeGC(), allocator(__VA_ARGS__))

#define STACK_LIMIT 512
#define GC_TRESHOLD 512

#define BINARY_OP(op)                \
    do {                             \
        auto op2 = AS_NUMBER(pop()); \
        auto op1 = AS_NUMBER(pop()); \
        push(NUMBER(op1 op op2));    \
    } while (false)  // only execute once

#define BINARY_ADD()                               \
    auto op2 = pop();                              \
    auto op1 = pop();                              \
    if (IS_NUMBER(op1) && IS_NUMBER(op2)) {        \
        auto v1 = AS_NUMBER(op1);                  \
        auto v2 = AS_NUMBER(op2);                  \
        push(NUMBER(v1 + v2));                     \
    } else if (IS_STRING(op1) && IS_STRING(op2)) { \
        auto s1 = AS_CPPSTRING(op1);               \
        auto s2 = AS_CPPSTRING(op2);               \
        push(ALLOC_STRING(s1 + s2));               \
    }

#define COMPARE_VALUES(op, v1, v2) \
    bool res;                      \
    switch (op) {                  \
        case 0:                    \
            res = v1 < v2;         \
            break;                 \
        case 1:                    \
            res = v1 > v2;         \
            break;                 \
        case 2:                    \
            res = v1 == v2;        \
            break;                 \
        case 3:                    \
            res = v1 >= v2;        \
            break;                 \
        case 4:                    \
            res = v1 <= v2;        \
            break;                 \
        case 5:                    \
            res = v1 != v2;        \
            break;                 \
    }                              \
    push(BOOLEAN(res));

struct Frame {
    uint8_t *ra;         
    EvaValue *bp;        
    FunctionObject *fn;  
};

class EvaVM {
   private:
    std::set<Traceable *> getGCRoots();
    std::set<Traceable *> getStackGCRoots();
    std::set<Traceable *> getConstantGCRoots();
    std::set<Traceable *> getGlobalGCRoots();

   public:
    EvaVM()
        : global(std::make_shared<Global>()),
          parser(std::make_unique<EvaParser>()),
          compiler(std::make_unique<EvaCompiler>(global)),
          collector(std::make_unique<EvaCollector>()) {
        setGlobalVariables();
    };

    ~EvaVM() { Traceable::cleanup(); };

    EvaValue peek(size_t offset = 0);

    void push(const EvaValue &value);

    void setGlobalVariables();

    EvaValue pop();

    void popN(size_t count);

    void maybeGC();

    EvaValue exec(const std::string &program);

    EvaValue eval();

    uint8_t *ip;

    EvaValue *sp;

    EvaValue *bp;

    std::array<EvaValue, STACK_LIMIT> stack;

    std::stack<Frame> callStack;

    std::vector<EvaValue> constants;

    std::shared_ptr<Global> global;

    std::unique_ptr<EvaParser> parser;

    std::unique_ptr<EvaCompiler> compiler;

    std::unique_ptr<EvaCollector> collector;

    FunctionObject *fn;

    void DebugDumpStack(uint8_t op);
};

#endif