#include "EvaVM.h"

using syntax::EvaParser;

EvaValue EvaVM::exec(const std::string &program) {
    auto ast = parser->parse("(begin " + program + ")");
    log(ast.number);
    compiler->compile(ast);
    fn = compiler->getMainFunction();
    ip = &fn->co->code[0];
    sp = &stack[0];
    bp = sp;
    compiler->disassembleByteCode();
    return eval();
}

EvaValue EvaVM::eval() {
    for (;;) {
        maybeGC();
        auto opcode = READ_BYTE();
        DebugDumpStack(opcode);
        switch (opcode) {
            case OP_HALT:
                return pop();

            case OP_CONST:
                // auto constIndex = READ_BYTE();
                // auto constant = constants[constIndex];
                // constants[READ_BYTE()]
                EvaVM::push(GET_CONST());
                break;
            case OP_ADD: {
                auto op2 = pop();
                auto op1 = pop();
                if (IS_NUMBER(op1) && IS_NUMBER(op2)) {
                    auto v1 = AS_NUMBER(op1);
                    auto v2 = AS_NUMBER(op2);
                    push(NUMBER(v1 + v2));
                } else if (IS_STRING(op1) && IS_STRING(op2)) {
                    auto s1 = AS_CPPSTRING(op1);
                    auto s2 = AS_CPPSTRING(op2);
                    push(MEM(ALLOC_STRING, s1 + s2));
                }
                break;
            }
            case OP_SUB: {
                BINARY_OP(-);
                break;
            }
            case OP_MUL: {
                BINARY_OP(*);
                break;
            }
            case OP_DIV: {
                BINARY_OP(/);
                break;
            }
            case OP_COMPARE: {
                auto op = READ_BYTE();
                auto op2 = pop();
                auto op1 = pop();

                if (IS_NUMBER(op1) && IS_NUMBER(op2)) {
                    auto v1 = AS_NUMBER(op1);
                    auto v2 = AS_NUMBER(op2);
                    COMPARE_VALUES(op, v1, v2);
                }

                else if (IS_STRING(op1) && IS_STRING(op2)) {
                    auto s1 = AS_STRING(op1);
                    auto s2 = AS_STRING(op2);
                    COMPARE_VALUES(op, s1, s2);
                }
                break;
            }

            case OP_JMP_IF_FALSE: {
                auto cond = AS_BOOLEAN(pop());
                auto address = READ_SHORT();

                if (!cond) {
                    ip = TO_ADDRESS(address);
                }
                break;
            }

            case OP_JUMP: {
                ip = TO_ADDRESS(READ_SHORT());
                break;
            }

            case OP_GET_GLOBAL: {
                auto globalIndex = READ_BYTE();
                push(global->get(globalIndex).value);
                break;
            }

            case OP_SET_GLOBAL: {
                auto globalIndex = READ_BYTE();
                auto value = pop();
                global->set(globalIndex, value);
                break;
            }
            case OP_POP:
                pop();
                break;

            case OP_GET_LOCAL: {
                auto localIndex = READ_BYTE();
                if (localIndex < 0 || localIndex >= stack.size()) {
                    DIE << "OP_GET_LOCAL: invalide variable index: " << (int)localIndex;
                }
                push(bp[localIndex]);
                break;
            }
            case OP_SET_LOCAL: {
                auto localIndex = READ_BYTE();
                auto value = peek(0);
                if (localIndex < 0 || localIndex >= stack.size()) {
                    DIE << "OP_GET_LOCAL: invalide variable index: " << (int)localIndex;
                }
                bp[localIndex] = value;
                break;
            }
            case OP_GET_CELL: {
                auto cellIndex = READ_BYTE();
                push(fn->cells[cellIndex]->value);
                break;
            }
            case OP_LOAD_CELL: {
                auto cellIndex = READ_BYTE();
                push(CELL(fn->cells[cellIndex]));
                break;
            }
            case OP_SET_CELL: {
                auto cellIndex = READ_BYTE();
                auto value = peek(0);

                if (fn->cells.size() <= cellIndex) {
                    fn->cells.push_back(AS_CELL(MEM(ALLOC_CELL, value)));
                } else {
                    fn->cells[cellIndex]->value = value;
                }
                break;
            }
            case OP_MAKE_FUNCTION: {
                auto co = AS_CODE(pop());
                auto cellsCount = READ_BYTE();

                auto fnValue = MEM(ALLOC_FUNCTION, co);
                auto fn = AS_FUNCTION(fnValue);

                for (auto i = 0; i < cellsCount; ++i) {
                    fn->cells.push_back(AS_CELL(pop()));
                }
                push(fnValue);
                break;
            }
            case OP_SCOPE_EXIT: {
                auto count = READ_BYTE();
                *(sp - 1 - count) = peek(0);
                popN(count);
                break;
            }

            case OP_CALL: {
                auto argsCount = READ_BYTE();
                EvaValue fnValue = peek(argsCount);
                if (IS_NATIVE(fnValue)) {
                    AS_NATIVE(fnValue)->function();
                    auto result = pop();
                    popN(argsCount + 1);
                    push(result);
                    break;
                }
                auto callee = AS_FUNCTION(fnValue);
                callStack.push(Frame{ip, bp, fn});
                fn = callee;
                fn->cells.resize(fn->co->freeCount);
                bp = sp - argsCount - 1;
                ip = &callee->co->code[0];
                break;
            }
            case OP_RETURN: {
                auto callerFrame = callStack.top();
                ip = callerFrame.ra;
                bp = callerFrame.bp;
                fn = callerFrame.fn;
                callStack.pop();
                break;
            }
            case OP_NEW: {
                auto classObject = AS_CLASS(pop());
                auto instance = MEM(ALLOC_INSTANCE, classObject);
                auto ctorValue = classObject->getProp("constructor");
                push(ctorValue);
                push(instance);
                break;
            }
            case OP_GET_PROP: {
                auto prop = AS_CPPSTRING(GET_CONST());
                auto object = pop();
                if (IS_INSTANCE(object)) {
                    push(AS_INSTANCE(object)->getProp(prop));
                } else if (IS_CLASS(object)) {
                    push(AS_CLASS(object)->getProp(prop));
                } else {
                    DIE << "[EvaVM]: Unknown object for OP_GET_PROP " << prop;
                }
                break;
            }
            case OP_SET_PROP: {
                auto prop = AS_CPPSTRING(GET_CONST());
                auto instance = AS_INSTANCE(pop());
                auto value = pop();
                push(instance->properties[prop] = value);
                break;
            }
            default:
                DIE << "Unknown Opcode: " << std::hex << opcode;
        }
    }
}

EvaValue EvaVM::pop() {
    if (sp == stack.begin()) {
        DIE << "pop(): empty stack.\n";
    }
    --sp;
    return *sp;
}

EvaValue EvaVM::peek(size_t offset) {
    if (stack.size() == 0) {
        DIE << "peek(): empty stack.\n";
    }

    return *(sp - 1 - offset);
}

void EvaVM::push(const EvaValue &value) {
    if ((size_t)(sp - stack.begin()) == STACK_LIMIT) {
        std::cout << "test" << std::endl;
        DIE << "push(): Stack overflow.\n";
    }

    *sp = value;
    sp++;
}

void EvaVM::setGlobalVariables() {
    global->addNativeFunction(
        "native-square",
        [&]() {
            auto x = AS_NUMBER(peek(0));
            push(NUMBER(x * x));
        },
        1);

    global->addNativeFunction(
        "sum",
        [&]() {
            auto v2 = AS_NUMBER(peek(0));
            auto v1 = AS_NUMBER(peek(1));
            push(NUMBER(v1 + v2));
        },
        2);

    global->addConst("VERSION", 1);
}

void EvaVM::popN(size_t count) {
    if (stack.size() == 0) {
        DIE << "popN(): empty stack.\n";
    }

    sp -= count;
}

void EvaVM::DebugDumpStack(uint8_t op) {
    std::cout << "\n========= Stack ===========\n";
    std::cout << ">> op: OP_" << opcodeToString(op) << std::endl;
    if (sp == stack.begin()) {
        std::cout << "(empty)";
    }

    auto csp = sp - 1;

    while (csp >= stack.begin()) {
        std::cout << *csp-- << "\n";
    }

    std::cout << "\n";
}

void EvaVM::maybeGC() {
    if (Traceable::bytesAllocated < GC_TRESHOLD) {
        return;
    }

    auto roots = getGCRoots();

    if (roots.size() == 0) {
        return;
    }

    std::cout << "------------ Before GC Stats ------------";
    Traceable::printStats();
    collector->gc(roots);
    std::cout << "------------ After GC Stats ------------";
    Traceable::printStats();
}

std::set<Traceable *> EvaVM::getGCRoots() {
    auto roots = getStackGCRoots();

    const auto constantRoots = getConstantGCRoots();
    roots.insert(constantRoots.begin(), constantRoots.end());

    const auto globalRoots = getGlobalGCRoots();
    roots.insert(globalRoots.begin(), globalRoots.end());

    return roots;
}

std::set<Traceable *> EvaVM::getStackGCRoots() {
    std::set<Traceable *> roots;
    auto stackEntry = sp;
    while (stackEntry-- != stack.begin()) {
        if (IS_OBJECT(*stackEntry)) {
            roots.insert((Traceable *)stackEntry->object);
        }
    }
    return roots;
}

std::set<Traceable *> EvaVM::getGlobalGCRoots() {
    std::set<Traceable *> roots;
    for (const auto &global : global->globals) {
        if (IS_OBJECT(global.value)) {
            roots.insert((Traceable *)global.value.object);
        }
    }

    return roots;
}

std::set<Traceable *> EvaVM::getConstantGCRoots() {
    return compiler->getConstantObject();
}