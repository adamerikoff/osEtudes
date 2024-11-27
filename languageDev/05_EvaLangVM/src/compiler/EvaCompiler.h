#ifndef EvaCompiler_h
#define EvaCompiler_h

#include "../includes.h"
#include "../bytecode/OpCode.h"
#include "../disassembler/EvaDisassembler.h"
#include "../logger/Logger.h"
#include "../parser/EvaParser.h"
#include "../vm/EvaValue.h"
#include "Scope.h"

#define ALLOC_CONST(tester, converter, allocator, value)                        \
    for (auto itr = co->constants.begin(); itr != co->constants.end(); ++itr) { \
        if (!tester(*itr)) {                                                    \
            continue;                                                           \
        }                                                                       \
                                                                                \
        if (converter(*itr) == value) {                                         \
            return std::distance(co->constants.begin(), itr);                   \
        }                                                                       \
    }                                                                           \
    co->addConst(allocator(value));

#define GEN_BINARY_OP(op) \
    gen(exp.list[1]);     \
    gen(exp.list[2]);     \
    emit(op);

#define FUNCTION_CALL(exp)                       \
    gen(exp.list[0]);                            \
    for (auto i = 1; i < exp.list.size(); ++i) { \
        gen(exp.list[i]);                        \
    }                                            \
    emit(OP_CALL);                               \
    emit(exp.list.size() - 1);

#define SAVE_AS_GLOBAL_OR_LOCAL(name)       \
    if (isGlobalScope()) {                  \
        global->define(name);               \
        emit(OP_SET_GLOBAL);                \
        emit(global->getGlobalIndex(name)); \
    } else {                                \
        co->addLocal(name);                 \
    }

class EvaCompiler {
   private:
    void emit(uint8_t code);
    size_t numericConstIdx(double value);
    size_t stringConstIdx(const std::string& value);
    size_t booleanConstIdx(const bool value);
    size_t getOffset();
    void patchJumpAddress(size_t offset, uint16_t value);
    void writeByteOffset(size_t offset, uint8_t value);
    std::shared_ptr<Global> global;
    std::unique_ptr<EvaDisassembler> disassembler;
    void blockEnter();
    void blockExit();
    bool isGlobalScope() const;
    bool isFunctionBody() const;
    bool isBlock(const Exp& exp) const;
    bool isDeclaration(const Exp& exp) const;
    bool isVarDeclaration(const Exp& exp) const;
    bool isFuncDeclaration(const Exp& exp) const;
    bool isTaggedList(const Exp& exp,
                      const std::string& tag) const;
    bool isLambdaDeclaration(const Exp& exp) const;
    bool isClassDeclaration(const Exp& exp) const;
    bool isProp(const Exp& exp) const;
    size_t getVarsCountOnScopeExit();
    EvaValue creatCodeObjectValue(const std::string& name, size_t arity = 0);

    FunctionObject* main;
    void compileFunction(const Exp& exp,
                         const std::string fnName,
                         const Exp& params,
                         const Exp& body);
    void analyze(const Exp& exp,
                 std::shared_ptr<Scope> scope);

    std::map<const Exp*, std::shared_ptr<Scope>> scopeInfo_;
    std::stack<std::shared_ptr<Scope>> scopeStack_;
    ClassObject* getClassByName(const std::string& name) const;

   public:
    EvaCompiler(std::shared_ptr<Global> global)
        : global(global),
          disassembler(std::make_unique<EvaDisassembler>(global)){};

    void disassembleByteCode();

    void compile(const Exp& exp);

    void gen(const Exp& exp);
    FunctionObject* getMainFunction() const;

    CodeObject* co;
    std::vector<CodeObject*> codeObjects_;
    std::set<Traceable*>& getConstantObject();
    std::vector<ClassObject*> classObjects_;
    ClassObject* classObject_;
    std::set<Traceable*> constantObjects_;
};

static std::map<std::string, uint8_t> compareOps_{
    {"<", 0},
    {">", 1},
    {"==", 2},
    {">=", 3},
    {"<=", 4},
    {"!=", 5},
};

#endif