#include "EvaCompiler.h"

void EvaCompiler::compile(const Exp& exp) {
    co = AS_CODE(creatCodeObjectValue("main"));
    main = AS_FUNCTION(ALLOC_FUNCTION(co));

    constantObjects_.insert((Traceable*)main);

    analyze(exp, nullptr);
    gen(exp);
    emit(OP_HALT);
}

void EvaCompiler::analyze(const Exp& exp,
                          std::shared_ptr<Scope> scope) {
    switch (exp.type) {
        case ExpType::SYMBOL: {
            if (exp.string == "true" || exp.string == "false" || exp.string == "null") {
                // do nothing
            } else {
                scope->maybePromote(exp.string);
                LOG(exp.string);
                LOG((int)scope->allocInfo[exp.string]);
            }
            break;
        }
        case ExpType::LIST: {
            auto tag = exp.list[0];

            if (tag.type == ExpType::SYMBOL) {
                auto op = tag.string;

                if (op == "begin") {
                    auto newScope =
                        std::make_shared<Scope>(scope == nullptr
                                                    ? ScopeType::GLOBAL
                                                    : ScopeType::BLOCK,
                                                scope);

                    scopeInfo_[&exp] = newScope;
                    for (auto i = 1; i < exp.list.size(); ++i) {
                        analyze(exp.list[i], newScope);
                    }
                }

                else if (op == "var") {
                    scope->addLocal(exp.list[1].string);
                    analyze(exp.list[2], scope);
                }

                else if (op == "def") {
                    auto fnName = exp.list[1].string;

                    scope->addLocal(fnName);

                    auto newScope = std::make_shared<Scope>(ScopeType::FUNCTION, scope);
                    scopeInfo_[&exp] = newScope;

                    newScope->addLocal(fnName);

                    auto arity = exp.list[2].list.size();

                    for (auto i = 0; i < arity; ++i) {
                        newScope->addLocal(exp.list[2].list[i].string);
                    }

                    analyze(exp.list[3], newScope);
                } else if (op == "lambda") {
                    auto newScope = std::make_shared<Scope>(ScopeType::FUNCTION, scope);
                    scopeInfo_[&exp] = newScope;

                    auto arity = exp.list[1].list.size();

                    for (auto i = 0; i < arity; ++i) {
                        newScope->addLocal(exp.list[1].list[i].string);
                    }

                    analyze(exp.list[2], newScope);
                } else if (op == "class") {
                    auto className = exp.list[1].string;
                    auto newScope = std::make_shared<Scope>(ScopeType::CLASS, scope);
                    scopeInfo_[&exp] = newScope;

                    scope->addLocal(className);

                    for (auto i = 3; i < exp.list.size(); ++i) {
                        analyze(exp.list[i], newScope);
                    }
                }

                else if (op == "prop") {
                    analyze(exp.list[1], scope);
                }

                else {
                    for (auto i = 1; i < exp.list.size(); ++i) {
                        analyze(exp.list[i], scope);
                    }
                }
            } else {
                for (auto i = 0; i < exp.list.size(); ++i) {
                    analyze(exp.list[i], scope);
                }
            }
            break;
        }
    }
}

void EvaCompiler::gen(const Exp& exp) {
    switch (exp.type) {
        case ExpType::NUMBER:
            emit(OP_CONST);
            emit(numericConstIdx(exp.number));
            break;

        case ExpType::STRING:
            emit(OP_CONST);
            emit(stringConstIdx(exp.string));
            break;

        case ExpType::SYMBOL:
            if (exp.string == "true" || exp.string == "false") {
                emit(OP_CONST);
                emit(booleanConstIdx(exp.string == "true" ? true : false));
            } else {
                auto varName = exp.string;

                auto opCodeGetter = scopeStack_.top()->getNameGetter(varName);

                emit(opCodeGetter);

                switch (opCodeGetter) {
                    case OP_GET_LOCAL: {
                        emit(co->getLocalIndex(varName));
                        break;
                    }
                    case OP_GET_CELL: {
                        emit(co->getCellIndex(varName));
                        break;
                    }
                    default: {
                        if (!global->exists(varName)) {
                            DIE << "[EvaCompiler]: Reference error: " << varName;
                        }
                        emit(global->getGlobalIndex(varName));
                        break;
                    }
                }
            }
            break;

        case ExpType::LIST:
            auto tag = exp.list[0];

            if (tag.type == ExpType::SYMBOL) {
                auto op = tag.string;
                if (op == "+") {
                    GEN_BINARY_OP(OP_ADD);
                }

                else if (op == "-") {
                    GEN_BINARY_OP(OP_SUB);
                }

                else if (op == "*") {
                    GEN_BINARY_OP(OP_MUL);
                }

                else if (op == "/") {
                    GEN_BINARY_OP(OP_DIV);
                }

                else if (compareOps_.count(op) != 0) {
                    gen(exp.list[1]);
                    gen(exp.list[2]);
                    emit(OP_COMPARE);
                    emit(compareOps_[op]);
                }

                else if (op == "if") {
                    gen(exp.list[1]);
                    emit(OP_JMP_IF_FALSE);

                    emit(0);
                    emit(0);

                    auto elseJmpAddr = getOffset() - 2;

                    gen(exp.list[2]);
                    emit(OP_JUMP);

                    emit(0);
                    emit(0);

                    auto endAddr = getOffset() - 2;

                    auto elseBranchAddr = getOffset();
                    patchJumpAddress(elseJmpAddr, elseBranchAddr);

                    if (exp.list.size() == 4) {
                        gen(exp.list[3]);
                    }

                    auto endBranchAddr = getOffset();
                    patchJumpAddress(endAddr, endBranchAddr);
                }

                else if (op == "while") {
                    auto loopStartAddr = getOffset();

                    gen(exp.list[1]);
                    emit(OP_JMP_IF_FALSE);

                    emit(0);
                    emit(0);

                    auto loopEndJmpAddr = getOffset() - 2;

                    gen(exp.list[2]);

                    emit(OP_JUMP);
                    emit(0);
                    emit(0);
                    patchJumpAddress(getOffset() - 2, loopStartAddr);

                    auto loopEndAddr = getOffset() + 1;

                    patchJumpAddress(loopEndJmpAddr, loopEndAddr);
                }

                else if (op == "for") {
                    gen(exp.list[1]);

                    auto loopStartAddr = getOffset();
                    gen(exp.list[2]);

                    emit(OP_JMP_IF_FALSE);

                    emit(0);
                    emit(0);

                    auto loopEndJmpAddr = getOffset() - 2;

                    gen(exp.list[4]);

                    gen(exp.list[3]);

                    emit(OP_JUMP);
                    emit(0); 
                    emit(0);
                    patchJumpAddress(getOffset() - 2, loopStartAddr);

                    auto loopEndAddr = getOffset() + 1;

                    patchJumpAddress(loopEndJmpAddr, loopEndAddr);

                }

                else if (op == "var") {
                    const auto varName = exp.list[1].string;

                    auto opCodeSetter = scopeStack_.top()->getNameSetter(varName);

                    if (isLambdaDeclaration(exp.list[2])) {
                        compileFunction(exp.list[2],
                                        varName,
                                        exp.list[2].list[1],
                                        exp.list[2].list[2]);
                    } else {
                        gen(exp.list[2]);
                    }

                    switch (opCodeSetter) {
                        case OP_SET_GLOBAL: {
                            global->define(varName);
                            emit(OP_SET_GLOBAL);
                            emit(global->getGlobalIndex(varName));
                            break;
                        }
                        case OP_SET_CELL: {
                            co->cellNames.push_back(varName);
                            emit(OP_SET_CELL);
                            emit(co->cellNames.size() - 1);
                            emit(OP_POP);
                            break;
                        }
                        default: {
                            co->addLocal(varName);
                            break;
                        }
                    }
                }
                else if (op == "set") {
                    if (isProp(exp.list[1])) {
                        gen(exp.list[2]);       
                        gen(exp.list[1].list[1]);  

                        emit(OP_SET_PROP);  
                        emit(stringConstIdx(exp.list[1].list[2].string));
                    }
                    else {
                        const auto varName = exp.list[1].string;

                        const auto opCodeSetter = scopeStack_.top()->getNameSetter(varName);

                        gen(exp.list[2]);

                        switch (opCodeSetter) {
                            case OP_SET_LOCAL: {
                                emit(OP_SET_LOCAL);
                                emit(co->getLocalIndex(varName));
                                break;
                            }
                            case OP_SET_CELL: {
                                emit(OP_SET_CELL);
                                emit(co->getCellIndex(varName));
                                break;
                            }
                            default: {
                                auto globalIndex = global->getGlobalIndex(varName);
                                if (globalIndex == -1) {
                                    DIE << "Reference Error: " << varName << " is not defined.";
                                }
                                emit(OP_SET_GLOBAL);
                                emit(globalIndex);
                                break;
                            }
                        }
                    }

                }

                else if (op == "begin") {
                    scopeStack_.push(scopeInfo_.at(&exp));
                    blockEnter();
                    for (auto i = 1; i < exp.list.size(); ++i) {
                        bool isLast = i == exp.list.size() - 1;

                        bool isDecl = isDeclaration(exp.list[i]);
                        gen(exp.list[i]);

                        if (!isLast && !isDecl) {
                            emit(OP_POP);
                        }
                    }
                    blockExit();
                    scopeStack_.pop();
                }
                else if (op == "def") {
                    auto fnName = exp.list[1].string;
                    compileFunction(
                        /*exp*/ exp,
                        /*name*/ fnName,
                        /*params*/ exp.list[2],
                        /*body*/ exp.list[3]);

                    if (classObject_ == nullptr) {
                        if (isGlobalScope()) {
                            global->define(fnName);
                            emit(OP_SET_GLOBAL);
                            emit(global->getGlobalIndex(fnName));
                        } else {
                            co->addLocal(fnName);
                        }
                    }

                } else if (op == "lambda") {
                    //
                    compileFunction(
                        /*exp*/ exp,
                        /*name*/ "lambda",
                        /*params*/ exp.list[1],
                        /*body*/ exp.list[2]);
                }

                else if (op == "class") {
                    auto name = exp.list[1].string;

                    auto superClass = exp.list[2].string == "null"
                                          ? nullptr
                                          : getClassByName(exp.list[2].string);

                    auto cls = ALLOC_CLASS(name, superClass);
                    auto classObject = AS_CLASS(cls);

                    classObjects_.push_back(classObject);

                    constantObjects_.insert((Traceable*)classObject);

                    co->addConst(cls);

                    global->define(name);

                    global->set(global->getGlobalIndex(name), cls);

                    if (exp.list.size() > 3) {
                        auto prevClassObject = classObject_;
                        classObject_ = classObject;

                        scopeStack_.push(scopeInfo_.at(&exp));
                        for (auto i = 3; i < exp.list.size(); ++i) {
                            gen(exp.list[i]);
                        }
                        scopeStack_.pop();

                        classObject_ = prevClassObject;
                    }

                } else if (op == "new") {
                    auto className = exp.list[1].string;
                    auto cls = getClassByName(className);

                    if (cls == nullptr) {
                        DIE << "[EvaCompiler]: Unknown class " << cls;
                    }

                    emit(OP_GET_GLOBAL);
                    emit(global->getGlobalIndex(className));

                    emit(OP_NEW);

                    for (auto i = 2; i < exp.list.size(); ++i) {
                        gen(exp.list[i]);
                    }

                    emit(OP_CALL);
                    emit(AS_FUNCTION(cls->getProp("constructor"))->co->arity);
                } else if (op == "prop") {
                    gen(exp.list[1]);
                    emit(OP_GET_PROP);
                    emit(stringConstIdx(exp.list[2].string));
                } else if (op == "super") {
                    const auto className = exp.list[1].string;
                    const auto cls = getClassByName(className);

                    if (cls == nullptr) {
                        DIE << "[EvaCompiler]: Unknown class "
                            << cls;
                    }

                    if (cls->superClass == nullptr) {
                        DIE << "[EvaCompiler]: Class "
                            << cls->name
                            << " doesn't have super class";
                    }

                    emit(OP_GET_GLOBAL);
                    emit(global->getGlobalIndex(cls->superClass->name));
                }
                
                else {
                    FUNCTION_CALL(exp)
                }
            }
            else {
                FUNCTION_CALL(exp)
            }
            break;
    }
}

void EvaCompiler::writeByteOffset(size_t offset, uint8_t value) {
    co->code[offset] = value;
}

void EvaCompiler::patchJumpAddress(size_t offset, uint16_t value) {
    writeByteOffset(offset, (value >> 8) & 0xff);
    writeByteOffset(offset + 1, value & 0xff);
}

size_t EvaCompiler::getOffset() {
    return co->code.size();
}

void EvaCompiler::emit(uint8_t code) {
    co->code.push_back(code);
}

size_t EvaCompiler::numericConstIdx(const double value) {
    ALLOC_CONST(IS_NUMBER, AS_NUMBER, NUMBER, value);

    return co->constants.size() - 1;
}

size_t EvaCompiler::stringConstIdx(const std::string& value) {
    ALLOC_CONST(IS_STRING, AS_CPPSTRING, ALLOC_STRING, value);
    constantObjects_.insert((Traceable*)co->constants.back().object);
    return co->constants.size() - 1;
}

size_t EvaCompiler::booleanConstIdx(const bool value) {
    ALLOC_CONST(IS_BOOLEAN, AS_BOOLEAN, BOOLEAN, value);

    return co->constants.size() - 1;
}

void EvaCompiler::disassembleByteCode() {
    for (auto& co_ : codeObjects_) {
        disassembler->disassemble(co_);
    }
}

void EvaCompiler::blockEnter() {
    co->scopeLevel++;
}
void EvaCompiler::blockExit() {
    auto varsCount = getVarsCountOnScopeExit();

    if (varsCount > 0 || co->arity > 0) {
        emit(OP_SCOPE_EXIT);

        if (isFunctionBody()) {
            varsCount += co->arity + 1;
        }
        emit(varsCount);
    }
    co->scopeLevel--;
}

bool EvaCompiler::isGlobalScope() const {
    return co->name == "main" && co->scopeLevel == 1;
}

bool EvaCompiler::isFunctionBody() const {
    return co->name != "main" && co->scopeLevel == 1;
}

bool EvaCompiler::isDeclaration(const Exp& exp) const {
    return isVarDeclaration(exp) || isFuncDeclaration(exp) || isClassDeclaration(exp);
}

bool EvaCompiler::isClassDeclaration(const Exp& exp) const {
    return isTaggedList(exp, "class");
}

bool EvaCompiler::isVarDeclaration(const Exp& exp) const {
    return isTaggedList(exp, "var");
}

bool EvaCompiler::isTaggedList(const Exp& exp,
                               const std::string& tag) const {
    return (exp.type == ExpType::LIST &&
            exp.list[0].type == ExpType::SYMBOL &&
            exp.list[0].string == tag);
}

bool EvaCompiler::isBlock(const Exp& exp) const {
    return isTaggedList(exp, "begin");
}

bool EvaCompiler::isLambdaDeclaration(const Exp& exp) const {
    return isTaggedList(exp, "lambda");
}

bool EvaCompiler::isFuncDeclaration(const Exp& exp) const {
    return isTaggedList(exp, "def");
}

bool EvaCompiler::isProp(const Exp& exp) const {
    return isTaggedList(exp, "prop");
}

size_t EvaCompiler::getVarsCountOnScopeExit() {
    auto varsCount = 0;

    if (co->locals.size() > 0) {
        while (co->locals.back().scopeLevel == co->scopeLevel) {
            // remove the locals from the co here
            // vm doesnt need them
            co->locals.pop_back();
            varsCount++;
        }
    }

    return varsCount;
}

EvaValue EvaCompiler::creatCodeObjectValue(const std::string& name, size_t arity) {
    auto coValue = ALLOC_CODE(name, arity);
    auto co = AS_CODE(coValue);

    codeObjects_.push_back(co);

    constantObjects_.insert((Traceable*)co);
    return coValue;
}

std::set<Traceable*>& EvaCompiler::getConstantObject() {
    return constantObjects_;
}

FunctionObject* EvaCompiler::getMainFunction() const { return main; }

ClassObject* EvaCompiler::getClassByName(const std::string& name) const {
    for (const auto& classObject : classObjects_) {
        if (classObject->name == name) {
            return classObject;
        }
    }
    return nullptr;
}

void EvaCompiler::compileFunction(const Exp& exp,
                                  const std::string fnName,
                                  const Exp& params,
                                  const Exp& body) {
    auto scopeInfo = scopeInfo_.at(&exp);
    scopeStack_.push(scopeInfo);

    auto arity = params.list.size();

    auto prevCo = co;

    auto coValue = creatCodeObjectValue(classObject_ != nullptr
                                            ? (classObject_->name + "." + fnName)
                                            : fnName,
                                        arity);
    co = AS_CODE(coValue);

    co->freeCount = scopeInfo->free.size();

    co->cellNames.reserve(scopeInfo->free.size() +
                          scopeInfo->cells.size());

    co->cellNames.insert(co->cellNames.end(),
                         scopeInfo->free.begin(),
                         scopeInfo->free.end());

    co->cellNames.insert(co->cellNames.end(),
                         scopeInfo->cells.begin(),
                         scopeInfo->cells.end());

    prevCo->addConst(coValue);
    co->addLocal(fnName);

    auto itr = params.list.begin();
    while (itr != params.list.end()) {
        auto argName = itr++->string;
        co->addLocal(argName);

        auto cellIndex = co->getCellIndex(argName);

        if (cellIndex != -1) {
            emit(OP_SET_CELL);
            emit(cellIndex);
        }
    }

    auto prevClassObject = classObject_;
    classObject_ = nullptr;
    gen(body);
    classObject_ = prevClassObject;

    if (!isBlock(body)) {
        emit(OP_SCOPE_EXIT);
        emit(arity + 1);
    }

    emit(OP_RETURN);

    if (classObject_ != nullptr) {
        auto fn = ALLOC_FUNCTION(co);
        constantObjects_.insert((Traceable*)AS_OBJECT(fn));
        co = prevCo;
        classObject_->properties[fnName] = fn;
    }

    else if (scopeInfo->free.size() == 0) {
        auto fn = ALLOC_FUNCTION(co);

        constantObjects_.insert((Traceable*)AS_OBJECT(fn));

        co = prevCo;
        co->addConst(fn);

        emit(OP_CONST);
        emit(co->constants.size() - 1);
    } else {
        co = prevCo;
        for (const auto& freeVar : scopeInfo->free) {
            emit(OP_LOAD_CELL);
            emit(prevCo->getCellIndex(freeVar));
        }

        emit(OP_CONST);
        emit(co->constants.size() - 1);

        emit(OP_MAKE_FUNCTION);
        emit(scopeInfo->free.size());
    }

    scopeStack_.pop();
}