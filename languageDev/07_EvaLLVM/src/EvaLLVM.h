#ifndef EVALLVM_h
#define EVALLVM_h

#include "includes.h"
#include "./Environment.h"
#include "./Logger.h"
#include "./parser/EvaParser.h"

struct ClassInfo {
    llvm::StructType* cls;
    llvm::StructType* parent;
    std::map<std::string, llvm::Type*> fieldsMap;
    std::map<std::string, llvm::Function*> methodsMap;
};

static const size_t VTABLE_INDEX = 0;

static const size_t RESERVED_FIELDS_COUNT = 1;

#define GEN_BINARY_OP(Op, varName)             \
    do {                                       \
        auto op1 = gen(exp.list[1], env);      \
        auto op2 = gen(exp.list[2], env);      \
        return builder->Op(op1, op2, varName); \
    } while (false)

class EvaLLVM {
public:
    EvaLLVM() : parser(std::make_unique<syntax::EvaParser>()) {
        moduleInit();
        setupExternalFunctions();
        setupGlobalEnvironment();
        setupTargetTriple();
    }

    void exec(const std::string& program) {
        auto ast = parser->parse("(begin " + program + ")");
        compile(ast);
        module->print(llvm::outs(), nullptr);
        saveModuleToFile("./out.ll");
    }

private:

    std::unique_ptr<llvm::LLVMContext> ctx;
    std::unique_ptr<llvm::Module> module;
    std::unique_ptr<llvm::IRBuilder<>> varsBuilder;
    std::unique_ptr<llvm::IRBuilder<>> builder;
    llvm::Function* fn;
    llvm::StructType* cls;
    std::map<std::string, ClassInfo> classMap_;
    std::unique_ptr<syntax::EvaParser> parser;
    std::shared_ptr<Environment> GlobalEnv;

    void moduleInit() {
        ctx = std::make_unique<llvm::LLVMContext>();
        module = std::make_unique<llvm::Module>("EvaLLVM", *ctx);
        builder = std::make_unique<llvm::IRBuilder<>>(*ctx);
        varsBuilder = std::make_unique<llvm::IRBuilder<>>(*ctx);
    }

    void compile(const Exp& ast) {
        fn = createFunction("main", llvm::FunctionType::get(builder->getInt32Ty(), false), GlobalEnv);
        createGlobalVar("VERSION", builder->getInt32(42));
        gen(ast, GlobalEnv);
        builder->CreateRet(builder->getInt32(0));
    }

    llvm::Function* createFunction(const std::string& fnName, llvm::FunctionType* fnType, std::shared_ptr<Environment> env) {
        auto fn = module->getFunction(fnName);
        if (fn == nullptr) {
            fn = createFunctionProto(fnName, fnType, env);
        }
        createFunctionBlock(fn);
        return fn;
    }

    llvm::Function* createFunctionProto(const std::string& fnName, llvm::FunctionType* fnType,
                                        std::shared_ptr<Environment> env) {
        auto fn = llvm::Function::Create(fnType, llvm::Function::ExternalLinkage, fnName, *module);
        verifyFunction(*fn);
        env->define(fnName, fn);
        return fn;
    }

    void createFunctionBlock(llvm::Function* fn) {
        auto entry = createBB("entry", fn);
        builder->SetInsertPoint(entry);
    }

    llvm::BasicBlock* createBB(const std::string& name, llvm::Function* fn = nullptr) {
        return llvm::BasicBlock::Create(*ctx, name, fn);
    }

    llvm::GlobalVariable* createGlobalVar(const std::string& name, llvm::Constant* init) {
        module->getOrInsertGlobal(name, init->getType());
        auto variable = module->getNamedGlobal(name);
        variable->setAlignment(llvm::MaybeAlign(4));
        variable->setConstant(false);
        variable->setInitializer(init);
        return variable;
    }
    llvm::Value* gen(const Exp& exp, std::shared_ptr<Environment> env) {
        switch (exp.type) {
            case ExpType::NUMBER: {
                return builder->getInt32(exp.number);
                break;
            }
            case ExpType::STRING: {
                auto re = std::regex("\\\\n");
                auto str = std::regex_replace(exp.string, re, "\n");
                return builder->CreateGlobalStringPtr(str);
                break;
            }
            case ExpType::SYMBOL: {
                if (exp.string == "true" || exp.string == "false") {
                    return builder->getInt1(exp.string == "true" ? true : false);
                }

                auto varName = exp.string;
                auto value = env->lookup(varName);

                if (auto localVar = llvm::dyn_cast<llvm::AllocaInst>(value)) {
                    return builder->CreateLoad(localVar->getAllocatedType(), localVar, varName.c_str());
                }
                else if (auto globalVar = llvm::dyn_cast<llvm::GlobalVariable>(value)) {
                    return builder->CreateLoad(globalVar->getInitializer()->getType(), globalVar, varName.c_str());
                }
                else {
                    return value;
                }
                break;
            }
            case ExpType::LIST: {
                auto tag = exp.list[0];
                if (tag.type == ExpType::SYMBOL) {
                    auto op = tag.string;
                    if (op == "+") {
                        GEN_BINARY_OP(CreateAdd, "tmpadd");
                    } else if (op == "-") {
                        GEN_BINARY_OP(CreateSub, "tmpsub");
                    } else if (op == "*") {
                        GEN_BINARY_OP(CreateMul, "tmpadd");
                    } else if (op == "/") {
                        GEN_BINARY_OP(CreateSDiv, "tmpdiv");
                    }
                    else if (op == "<") {
                        GEN_BINARY_OP(CreateICmpULT, "tmpcmp");
                    } else if (op == "<=") {
                        GEN_BINARY_OP(CreateICmpULE, "tmpcmp");
                    } else if (op == ">") {
                        GEN_BINARY_OP(CreateICmpUGT, "tmpcmp");
                    } else if (op == ">=") {
                        GEN_BINARY_OP(CreateICmpUGE, "tmpcmp");
                    } else if (op == "==") {
                        GEN_BINARY_OP(CreateICmpEQ, "tmpcmp");
                    } else if (op == "!=") {
                        GEN_BINARY_OP(CreateICmpNE, "tmpcmp");
                    }
                    else if (op == "if") {
                        auto cond = gen(exp.list[1], env);

                        // Create basic blocks
                        auto thenBlock = llvm::BasicBlock::Create(*ctx, "then", fn);
                        auto elseBlock = llvm::BasicBlock::Create(*ctx, "else");
                        auto ifEndBlock = llvm::BasicBlock::Create(*ctx, "ifend");

                        // Create conditional branch
                        builder->CreateCondBr(cond, thenBlock, elseBlock);

                        // Generate 'then' block
                        builder->SetInsertPoint(thenBlock);
                        auto thenRes = gen(exp.list[2], env);
                        builder->CreateBr(ifEndBlock);

                        // Save the current block as 'thenBlock' (LLVM updates it internally)
                        thenBlock = builder->GetInsertBlock();

                        // Add and generate 'else' block
                        fn->getBasicBlockList().push_back(elseBlock); // Not needed; elseBlock will be attached dynamically
                        builder->SetInsertPoint(elseBlock);
                        auto elseRes = gen(exp.list[3], env);
                        builder->CreateBr(ifEndBlock);

                        // Save the current block as 'elseBlock'
                        elseBlock = builder->GetInsertBlock();

                        // Create 'ifend' block
                        fn->getBasicBlockList().push_back(ifEndBlock); // Not needed; handled by Create method
                        builder->SetInsertPoint(ifEndBlock);

                        // Create PHI node
                        auto phi = builder->CreatePHI(thenRes->getType(), 2, "tmpif");
                        phi->addIncoming(thenRes, thenBlock);
                        phi->addIncoming(elseRes, elseBlock);

                        return phi;
                    }
                    else if (op == "while") {
                        // Create 'cond', 'body', and 'loopend' blocks
                        auto condBlock = llvm::BasicBlock::Create(*ctx, "cond", fn);
                        auto bodyBlock = llvm::BasicBlock::Create(*ctx, "body");
                        auto loopEndBlock = llvm::BasicBlock::Create(*ctx, "loopend");

                        // Branch to condition block initially
                        builder->CreateBr(condBlock);

                        // Generate 'cond' block
                        builder->SetInsertPoint(condBlock);
                        auto cond = gen(exp.list[1], env);
                        builder->CreateCondBr(cond, bodyBlock, loopEndBlock);

                        // Generate 'body' block
                        fn->getBasicBlockList().push_back(bodyBlock); // Not needed; dynamically attached
                        builder->SetInsertPoint(bodyBlock);
                        gen(exp.list[2], env);
                        builder->CreateBr(condBlock);

                        // Generate 'loopend' block
                        fn->getBasicBlockList().push_back(loopEndBlock); // Not needed; handled dynamically
                        builder->SetInsertPoint(loopEndBlock);

                        return builder->getInt32(0);
                    }

                    else if (op == "def") {
                        return compileFunction(exp, exp.list[1].string, env);
                    }
                    else if (op == "var") {
                        if (cls != nullptr) {
                            return builder->getInt32(0);
                        }
                        auto varNameDecl = exp.list[1];
                        auto varName = extractVarName(varNameDecl);
                        if (isNew(exp.list[2])) {
                            auto instance = createInstance(exp.list[2], env, varName);
                            return env->define(varName, instance);
                        }
                        auto init = gen(exp.list[2], env);
                        auto varTy = extractVarType(varNameDecl);
                        auto varBinding = allocVar(varName, varTy, env);
                        return builder->CreateStore(init, varBinding);
                    }
                    else if (op == "set") {
                        auto value = gen(exp.list[2], env);
                        if (isProp(exp.list[1])) {
                            auto instance = gen(exp.list[1].list[1], env);
                            auto fieldName = exp.list[1].list[2].string;
                            auto ptrName = std::string("p") + fieldName;
                            auto cls = (llvm::StructType*)(instance->getType()->getContainedType(0));
                            auto fieldIdx = getFieldIndex(cls, fieldName);
                            auto address = builder->CreateStructGEP(cls, instance, fieldIdx, ptrName);
                            builder->CreateStore(value, address);
                            return value;
                        }
                        else {
                            auto varName = exp.list[1].string;
                            auto varBinding = env->lookup(varName);
                            builder->CreateStore(value, varBinding);
                            return value;
                        }
                    }
                    else if (op == "begin") {
                        auto blockEnv = std::make_shared<Environment>(std::map<std::string, llvm::Value*>{}, env);
                        llvm::Value* blockRes;
                        for (auto i = 1; i < exp.list.size(); i++) {
                            blockRes = gen(exp.list[i], blockEnv);
                        }
                        return blockRes;
                    }
                    else if (op == "printf") {
                        auto printfFn = module->getFunction("printf");
                        std::vector<llvm::Value*> args{};
                        for (auto i = 1; i < exp.list.size(); i++) {
                            args.push_back(gen(exp.list[i], env));
                        }
                        return builder->CreateCall(printfFn, args);
                    }
                    else if (op == "class") {
                        auto name = exp.list[1].string;

                        auto parent = exp.list[2].string == "null" ? nullptr : getClassByName(exp.list[2].string);
                        cls = llvm::StructType::create(*ctx, name);
                        if (parent != nullptr) {
                            inheritClass(cls, parent);
                        } else {
                            classMap_[name] = {
                                cls,
                                parent,
                                {},
                                {}
                            };
                        }
                        buildClassInfo(cls, exp, env);
                        gen(exp.list[3], env);
                        cls = nullptr;
                        return builder->getInt32(0);
                    }
                    else if (op == "new") {
                        return createInstance(exp, env, "");
                    }
                    else if (op == "prop") {
                        auto instance = gen(exp.list[1], env);
                        auto fieldName = exp.list[2].string;
                        auto ptrName = std::string("p") + fieldName;
                        auto cls = (llvm::StructType*)(instance->getType()->getContainedType(0));
                        auto fieldIdx = getFieldIndex(cls, fieldName);
                        auto address = builder->CreateStructGEP(cls, instance, fieldIdx, ptrName);
                        return builder->CreateLoad(cls->getElementType(fieldIdx), address, fieldName);
                    }
                    else if (op == "method") {
                        auto methodName = exp.list[2].string;
                        llvm::StructType* cls;
                        llvm::Value* vTable;
                        llvm::StructType* vTableTy;
                        if (isSuper(exp.list[1])) {
                            auto className = exp.list[1].list[1].string;
                            cls = classMap_[className].parent;
                            auto parentName = std::string(cls->getName().data());
                            vTable = module->getNamedGlobal(parentName + "_vTable");
                            vTableTy = llvm::StructType::getTypeByName(*ctx, parentName + "_vTable");
                        }
                        else {
                            auto instance = gen(exp.list[1], env);
                            cls = (llvm::StructType*)(instance->getType()->getContainedType(0));
                            auto vTableAddr = builder->CreateStructGEP(cls, instance, VTABLE_INDEX);
                            vTable = builder->CreateLoad(cls->getElementType(VTABLE_INDEX), vTableAddr, "vt");
                            vTableTy = (llvm::StructType*)(vTable->getType()->getContainedType(0));
                        }
                        auto methodIdx = getMethodIndex(cls, methodName);
                        auto methodTy = (llvm::FunctionType*)vTableTy->getElementType(methodIdx);
                        auto methodAddr = builder->CreateStructGEP(vTableTy, vTable, methodIdx);
                        return builder->CreateLoad(methodTy, methodAddr);
                    }
                    else {
                        auto callable = gen(exp.list[0], env);
                        auto callableTy = callable->getType()->getContainedType(0);
                        std::vector<llvm::Value*> args{};
                        auto argIdx = 0;
                        if (callableTy->isStructTy()) {
                            auto cls = (llvm::StructType*)callableTy;
                            std::string className(cls->getName().data());
                            args.push_back(callable);
                            argIdx++;
                            callable = module->getFunction(className + "___call__");
                        }

                        auto fn = (llvm::Function*)callable;

                        for (auto i = 1; i < exp.list.size(); i++, argIdx++) {
                            auto argValue = gen(exp.list[i], env);
                            auto paramTy = fn->getArg(argIdx)->getType();
                            auto bitCastArgVal = builder->CreateBitCast(argValue, paramTy);
                            args.push_back(bitCastArgVal);
                        }
                        return builder->CreateCall(fn, args);
                    }
                }
                else {
                    auto loadedMethod = (llvm::LoadInst*)gen(exp.list[0], env);

                    auto fnTy = (llvm::FunctionType*)(loadedMethod->getPointerOperand()
                                                              ->getType()
                                                              ->getContainedType(0)
                                                              ->getContainedType(0));
                    std::vector<llvm::Value*> args{};
                    for (auto i = 1; i < exp.list.size(); i++) {
                        auto argValue = gen(exp.list[i], env);
                        auto paramTy = fnTy->getParamType(i - 1);
                        if (argValue->getType() != paramTy) {
                            auto bitCastArgVal = builder->CreateBitCast(argValue, paramTy);
                            args.push_back(bitCastArgVal);
                        } else {
                            args.push_back(argValue);
                        }
                    }

                    return builder->CreateCall(fnTy, loadedMethod, args);
                }
                break;
            }
        }
        return builder->getInt32(0);
    }

    void setupExternalFunctions() {
        auto bytePtrTy = builder->getInt8Ty()->getPointerTo();
        module->getOrInsertFunction("printf", llvm::FunctionType::get(builder->getInt32Ty(), bytePtrTy, true));
        module->getOrInsertFunction("GC_malloc", llvm::FunctionType::get(bytePtrTy, builder->getInt64Ty(), false));
    }

    void setupGlobalEnvironment() {
        std::map<std::string, llvm::Value*> globalObject{
                {"VERSION", builder->getInt32(42)},
        };
        std::map<std::string, llvm::Value*> globalRec{};

        for (const auto& entry : globalObject) {
            globalRec[entry.first] = createGlobalVar(entry.first, (llvm::Constant*)entry.second);
        }

        GlobalEnv = std::make_shared<Environment>(globalRec, nullptr);
    }

    void setupTargetTriple() {
        module->setTargetTriple("x86_64-pc-linux-gnu");
    }

    void saveModuleToFile(const std::string& fileName) {
        std::error_code errorCode;
        llvm::raw_fd_ostream outLL(fileName, errorCode);
        module->print(outLL, nullptr);
    }

    std::string extractVarName(const Exp& exp) {
        return exp.type == ExpType::LIST ? exp.list[0].string : exp.string;
    }

    llvm::Type* extractVarType(const Exp& exp) {
        return exp.type == ExpType::LIST ? getTypeFromString(exp.list[1].string) : builder->getInt32Ty();
    }

    llvm::Type* getTypeFromString(const std::string& type_) {
        if (type_ == "number") {
            return builder->getInt32Ty();
        }
        if (type_ == "string") {
            return builder->getInt8Ty()->getPointerTo();
        }
        return classMap_[type_].cls->getPointerTo();
    }

    bool hasReturnType(const Exp& fnExp) {
        return fnExp.list[3].type == ExpType::SYMBOL && fnExp.list[3].string == "->";
    }

    llvm::FunctionType* extractFunctionType(const Exp& fnExp) {
        auto params = fnExp.list[2];
        auto returnType = hasReturnType(fnExp) ? getTypeFromString(fnExp.list[4].string) : builder->getInt32Ty();
        std::vector<llvm::Type*> paramTypes{};
        for (auto& param : params.list) {
            auto paramName = extractVarName(param);
            auto paramTy = extractVarType(param);
            paramTypes.push_back(paramName == "self" ? (llvm::Type*)cls->getPointerTo() : paramTy);
        }
        return llvm::FunctionType::get(returnType, paramTypes, false);
    }

    llvm::Value* compileFunction(const Exp& fnExp, std::string fnName, std::shared_ptr<Environment> env) {
        auto params = fnExp.list[2];
        auto body = hasReturnType(fnExp) ? fnExp.list[5] : fnExp.list[3];
        auto prevFn = fn;
        auto prevBlock = builder->GetInsertBlock();
        auto origName = fnName;
        if (cls != nullptr) {
            fnName = std::string(cls->getName().data()) + "_" + fnName;
        }
        auto newFn = createFunction(fnName, extractFunctionType(fnExp), env);
        fn = newFn;
        auto idx = 0;
        auto fnEnv = std::make_shared<Environment>(std::map<std::string, llvm::Value*>{}, env);
        for (auto& arg : fn->args()) {
            auto param = params.list[idx++];
            auto argName = extractVarName(param);
            arg.setName(argName);
            auto argBinding = allocVar(argName, arg.getType(), fnEnv);
            builder->CreateStore(&arg, argBinding);
        }
        builder->CreateRet(gen(body, fnEnv));
        builder->SetInsertPoint(prevBlock);
        fn = prevFn;
        return newFn;
    }

    llvm::Value* allocVar(const std::string& name, llvm::Type* type_, std::shared_ptr<Environment> env) {
        varsBuilder->SetInsertPoint(&fn->getEntryBlock());
        auto varAlloc = varsBuilder->CreateAlloca(type_, 0, name.c_str());
        env->define(name, varAlloc);
        return varAlloc;
    }

    llvm::StructType* getClassByName(const std::string& name) {
        return llvm::StructType::getTypeByName(*ctx, name);
    }

    size_t getFieldIndex(llvm::StructType* cls, const std::string& fieldName) {
        auto fields = &classMap_[cls->getName().data()].fieldsMap;
        auto it = fields->find(fieldName);
        return std::distance(fields->begin(), it) + RESERVED_FIELDS_COUNT;
    }

    size_t getMethodIndex(llvm::StructType* cls, const std::string& methodName) {
        auto methods = &classMap_[cls->getName().data()].methodsMap;
        auto it = methods->find(methodName);
        return std::distance(methods->begin(), it);
    }

    llvm::Value* createInstance(const Exp& exp, std::shared_ptr<Environment> env, const std::string& name) {
        auto className = exp.list[1].string;
        auto cls = getClassByName(className);
        if (cls == nullptr) {
            DIE << "[EvaLLVM] Unknown class " << cls;
        }
        auto instance = mallocInstance(cls, name);
        auto ctor = module->getFunction(className + "_constructor");
        std::vector<llvm::Value*> args{instance};
        for (auto i = 2; i < exp.list.size(); i++) {
            args.push_back(gen(exp.list[i], env));
        }
        builder->CreateCall(ctor, args);
        return instance;
    }

    llvm::Value* mallocInstance(llvm::StructType* cls, const std::string& name) {
        auto typeSize = builder->getInt64(getTypeSize(cls));
        auto mallocPtr = builder->CreateCall(module->getFunction("GC_malloc"), typeSize, name);
        auto instance = builder->CreatePointerCast(mallocPtr, cls->getPointerTo());
        std::string className(cls->getName().data());
        auto vTableName = className + "_vTable";
        auto vTableAddr = builder->CreateStructGEP(cls, instance, VTABLE_INDEX);
        auto vTable = module->getNamedGlobal(vTableName);
        builder->CreateStore(vTable, vTableAddr);
        return instance;
    }

    size_t getTypeSize(llvm::Type* type_) {
        return module->getDataLayout().getTypeAllocSize(type_);
    }

    void inheritClass(llvm::StructType* cls, llvm::StructType* parent) {
        auto parentClassInfo = &classMap_[parent->getName().data()];
        classMap_[cls->getName().data()] = {
            cls,
            parent,
            parentClassInfo->fieldsMap,
            parentClassInfo->methodsMap,
        };
    }

    bool isTaggedList(const Exp& exp, const std::string tag) {
        return exp.type == ExpType::LIST && exp.list[0].type == ExpType::SYMBOL && exp.list[0].string == tag;
    }

    bool isVar(const Exp& exp) {
        return isTaggedList(exp, "var");
    }

    bool isDef(const Exp& exp) {
        return isTaggedList(exp, "def");
    }

    bool isNew(const Exp& exp) {
        return isTaggedList(exp, "new");
    }

    bool isProp(const Exp& exp) {
        return isTaggedList(exp, "prop");
    }

    bool isSuper(const Exp& exp) {
        return isTaggedList(exp, "super");
    }

    void buildVTable(llvm::StructType* cls) {
        std::string className(cls->getName().data());
        auto vTableName = className + "_vTable";
        auto vTableTy = llvm::StructType::getTypeByName(*ctx, vTableName);
        std::vector<llvm::Constant*> vTableMethods;
        std::vector<llvm::Type*> vTableMethodTys;
        for (auto& methodInfo : classMap_[className].methodsMap) {
            auto method = methodInfo.second;
            vTableMethods.push_back(method);
            vTableMethodTys.push_back(method->getType());
        }
        vTableTy->setBody(vTableMethodTys);
        auto vTableValue = llvm::ConstantStruct::get(vTableTy, vTableMethods);
        createGlobalVar(vTableName, vTableValue);
    }

    void buildClassBody(llvm::StructType* cls) {
        std::string className(cls->getName().data());
        auto classInfo = &classMap_[className];
        auto vTableName = className + "_vTable";
        auto vTableTy = llvm::StructType::create(*ctx, vTableName);
        auto clsFields = std::vector<llvm::Type*>{
                vTableTy->getPointerTo(),
        };
        for (const auto& fieldInfo : classInfo->fieldsMap) {
            clsFields.push_back(fieldInfo.second);
        }
        cls->setBody(clsFields, false);
        buildVTable(cls);
    }

    void buildClassInfo(llvm::StructType* cls, const Exp& clsExp, std::shared_ptr<Environment> env) {
        auto className = clsExp.list[1].string;
        auto classInfo = &classMap_[className];
        auto body = clsExp.list[3];
        for (auto i = 1; i < body.list.size(); i++) {
            auto exp = body.list[i];

            if (isVar(exp)) {
                auto varNameDecl = exp.list[1];

                auto fieldName = extractVarName(varNameDecl);
                auto fieldTy = extractVarType(varNameDecl);

                classInfo->fieldsMap[fieldName] = fieldTy;
            }

            else if (isDef(exp)) {
                auto methodName = exp.list[1].string;
                auto fnName = className + "_" + methodName;

                classInfo->methodsMap[methodName] = createFunctionProto(fnName, extractFunctionType(exp), env);
            }
        }
        buildClassBody(cls);
    }
};

#endif