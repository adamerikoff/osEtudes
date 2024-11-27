#ifndef EVAVALUE_H
#define EVAVALUE_H

#include "../includes.h"
#include "../logger/Logger.h"

enum class EvaValueType {
    NUMBER,
    BOOLEAN,
    OBJECT,
};

enum class ObjectType {
    STRING,
    CODE,
    NATIVE,
    FUNCTION,
    CELL,
    CLASS,
    INSTANCE,
};

struct Traceable {
    bool marked;
    size_t size;
    static void* operator new(size_t size) {
        void* object = ::operator new(size);
        ((Traceable*)object)->size = size;

        Traceable::objects.push_back((Traceable*)object);
        Traceable::bytesAllocated += size;

        return object;
    }

    static void operator delete(void* object, std::size_t sz) {
        Traceable::bytesAllocated -= ((Traceable*)object)->size;
        ::operator delete(object, sz);
    }

    static void cleanup() {
        for (auto& object : objects) {
            delete object;
        }
        objects.clear();
    }

    static void printStats() {
        std::cout << "---------------------\n";
        std::cout << "Memory stats:\n\n";
        std::cout << "Objects allocated : "
                  << std::dec
                  << Traceable::objects.size()
                  << std::endl;
        std::cout << "Bytes allocated : "
                  << std::dec
                  << Traceable::bytesAllocated
                  << std::endl
                  << std::endl;
    }

    static size_t bytesAllocated;
    static std::list<Traceable*> objects;
};

inline size_t Traceable::bytesAllocated{0};
inline std::list<Traceable*> Traceable::objects{};

struct Object : public Traceable {
    Object(ObjectType type)
        : type(type) {}
    ObjectType type;
};

struct EvaValue {
    EvaValueType type;
    union {
        double number;
        bool boolean;
        Object* object;
    };
};

struct ClassObject : public Object {
    ClassObject(const std::string& name, ClassObject* superClass)
        : Object(ObjectType::CLASS),
          name(name),
          properties{},
          superClass(superClass) {
    }
    std::string name;

    std::map<std::string, EvaValue> properties;

    ClassObject* superClass;

    inline EvaValue getProp(const std::string& prop) {
        if (properties.count(prop) != 0) {
            return properties[prop];
        }

        if (superClass == nullptr) {
            DIE << "Unresolved property " << prop << " in class " << name;
        }

        return superClass->getProp(prop);
    }

    inline void setProp(const std::string& prop, const EvaValue& value) {
        properties[prop] = value;
    }
};

struct StringObject : public Object {
    StringObject(const std::string& str)
        : Object(ObjectType::STRING), string(str) {}
    std::string string;
};

using NativeFn = std::function<void()>;

struct NativeObject : public Object {
    NativeObject(NativeFn function,
                 const std::string& name,
                 size_t arity)
        : Object(ObjectType::NATIVE),
          function(function),
          name(name),
          arity(arity) {}

    NativeFn function;

    std::string name;

    size_t arity;
};

struct LocalVar {
    std::string name;
    size_t scopeLevel;
};

struct InstanceObject : public Object {
    InstanceObject(ClassObject* cls)
        : Object(ObjectType::INSTANCE),
          cls(cls),
          properties{} {}

    ClassObject* cls;
    std::map<std::string, EvaValue> properties;

    EvaValue getProp(const std::string& prop) {
        if (properties.count(prop) != 0) {
            return properties[prop];
        }
        return cls->getProp(prop);
    }
};

struct CodeObject : public Object {
    CodeObject(const std::string& name, size_t arity)
        : Object(ObjectType::CODE), name(name), arity(arity) {}
    std::string name;

    size_t arity;
    std::vector<EvaValue> constants;
    std::vector<uint8_t> code;

    size_t scopeLevel = 0;
    std::vector<LocalVar> locals;

    std::vector<std::string> cellNames;

    size_t freeCount = 0;

    inline int getLocalIndex(const std::string& name) const {
        if (locals.size() > 0) {
            // have to go from behind to get the newest stack value
            for (auto itr = locals.rbegin(); itr != locals.rend(); ++itr) {
                if (itr->name == name) {
                    return std::distance(itr, locals.rend()) - 1;
                }
            }
        }

        return -1;
    };

    inline int getCellIndex(const std::string& name) const {
        if (cellNames.size() > 0) {
            // have to go from behind to get the newest stack value
            for (auto itr = cellNames.rbegin(); itr != cellNames.rend(); ++itr) {
                if (*itr == name) {
                    return std::distance(itr, cellNames.rend()) - 1;
                }
            }
        }

        return -1;
    };

    inline void addLocal(const std::string& name) {
        locals.push_back({name, scopeLevel});
    };

    inline void addConst(const EvaValue& value) {
        constants.push_back(value);
    };
};

struct CellObject : public Object {
    CellObject(EvaValue value)
        : Object(ObjectType::CELL),
          value(value) {}

    EvaValue value;
};

struct FunctionObject : public Object {
    FunctionObject(CodeObject* co)
        : Object(ObjectType::FUNCTION), co(co) {}

    CodeObject* co;

    std::vector<CellObject*> cells;
};
//------------------------------
//------------------------------
//------------------------------
// EASY CONSTRUCTORS
#define NUMBER(value) ((EvaValue){EvaValueType::NUMBER, .number = value})
#define BOOLEAN(value) ((EvaValue){EvaValueType::BOOLEAN, .boolean = value})
#define OBJECT(value) ((EvaValue){EvaValueType::OBJECT, .object = value})

#define ALLOC_STRING(value) ((EvaValue){EvaValueType::OBJECT, .object = (Object*)new StringObject(value)})
#define ALLOC_CODE(name, arity) ((EvaValue){EvaValueType::OBJECT, .object = (Object*)new CodeObject(name, arity)})
#define ALLOC_NATIVE(fn, name, arity) \
    ((EvaValue){EvaValueType::OBJECT, \
                .object = (Object*)new NativeObject(fn, name, arity)})
#define ALLOC_CELL(co) \
    ((EvaValue){EvaValueType::OBJECT, .object = (Object*)new CellObject(co)})

#define ALLOC_FUNCTION(co) \
    ((EvaValue){EvaValueType::OBJECT, .object = (Object*)new FunctionObject(co)})
#define ALLOC_CLASS(name, superClass) \
    ((EvaValue){EvaValueType::OBJECT, \
                .object = (Object*)new ClassObject(name, superClass)})

#define ALLOC_INSTANCE(cls) \
    ((EvaValue){EvaValueType::OBJECT, .object = (Object*)new InstanceObject(cls)})

#define CELL(cellObject) OBJECT((Object*)cellObject)
#define CLASS(classObject) OBJECT((Object*)classObject)
//------------------------------
//------------------------------
//------------------------------
// CONVERTERS: 
#define AS_NUMBER(evaValue) ((double)(evaValue).number)
#define AS_BOOLEAN(evaValue) ((bool)(evaValue).boolean)
#define AS_STRING(evaValue) ((StringObject*)(evaValue).object)
#define AS_OBJECT(evaValue) ((Object*)(evaValue).object)      
#define AS_CODE(evaValue) ((CodeObject*)(evaValue).object)    
#define AS_NATIVE(fnValue) ((NativeObject*)(fnValue).object)
#define AS_FUNCTION(evaValue) ((FunctionObject*)(evaValue).object)
#define AS_CPPSTRING(evaValue) (AS_STRING(evaValue)->string)  
#define AS_CELL(evaValue) ((CellObject*)(evaValue).object)    
#define AS_CLASS(evaValue) ((ClassObject*)(evaValue).object)
#define AS_INSTANCE(evaValue) ((InstanceObject*)(evaValue).object)
//-------------------------------
//------------------------------
//------------------------------
// VALUE-TESTER:
#define IS_NUMBER(evaValue) ((evaValue).type == EvaValueType::NUMBER)
#define IS_BOOLEAN(evaValue) ((evaValue).type == EvaValueType::BOOLEAN)
#define IS_OBJECT(evaValue) ((evaValue).type == EvaValueType::OBJECT)
#define IS_OBJECT_TYPE(evaValue, objectType) (IS_OBJECT(evaValue) && AS_OBJECT(evaValue)->type == objectType)
#define IS_STRING(evaValue) IS_OBJECT_TYPE(evaValue, ObjectType::STRING)
#define IS_CODE(evaValue) IS_OBJECT_TYPE(evaValue, ObjectType::CODE)
#define IS_NATIVE(fnValue) IS_OBJECT_TYPE(fnValue, ObjectType::NATIVE)
#define IS_FUNCTION(fnValue) IS_OBJECT_TYPE(fnValue, ObjectType::FUNCTION)
#define IS_CELL(evaValue) IS_OBJECT_TYPE(evaValue, ObjectType::CELL)
#define IS_CLASS(evaValue) IS_OBJECT_TYPE(evaValue, ObjectType::CLASS)
#define IS_INSTANCE(evaValue) IS_OBJECT_TYPE(evaValue, ObjectType::INSTANCE)

inline std::string evaValueToTypeString(const EvaValue& evaValue) {
    if (IS_NUMBER(evaValue)) {
        return "NUMBER";
    } else if (IS_BOOLEAN(evaValue)) {
        return "BOOLEAN";
    } else if (IS_STRING(evaValue)) {
        return "STRING";
    } else if (IS_CODE(evaValue)) {
        return "CODE";
    } else if (IS_NATIVE(evaValue)) {
        return "NATIVE";
    } else if (IS_FUNCTION(evaValue)) {
        return "FUNCTION";
    } else if (IS_CELL(evaValue)) {
        return "CELL";
    } else if (IS_CLASS(evaValue)) {
        return "CLASS";
    } else if (IS_INSTANCE(evaValue)) {
        return "INSTANCE";
    } else {
        DIE << "evaValueToTypeString: Unknown type" << (int)evaValue.type;
    }
    return "";
}

inline std::string evaValueToConstantString(const EvaValue& evaValue) {
    std::stringstream ss;
    if (IS_NUMBER(evaValue)) {
        ss << evaValue.number;
    } else if (IS_BOOLEAN(evaValue)) {
        ss << (evaValue.boolean == true ? "true" : "false");
    } else if (IS_STRING(evaValue)) {
        ss << '"' << AS_CPPSTRING(evaValue) << '"';
    } else if (IS_CODE(evaValue)) {
        auto code = AS_CODE(evaValue);
        ss << "code " << code << ": " << code->name << "/" << code->arity;
    } else if (IS_NATIVE(evaValue)) {
        auto fn = AS_NATIVE(evaValue);
        ss << fn->name << "/" << fn->arity;
    } else if (IS_FUNCTION(evaValue)) {
        auto fn = AS_FUNCTION(evaValue);
        ss << fn->co->name << "/" << fn->co->arity;
    } else if (IS_CELL(evaValue)) {
        auto cell = AS_CELL(evaValue);
        ss << "cell: " << evaValueToConstantString(cell->value);
    } else if (IS_CLASS(evaValue)) {
        auto cls = AS_CLASS(evaValue);
        ss << "class: " << cls->name;
    } else if (IS_INSTANCE(evaValue)) {
        auto instance = AS_INSTANCE(evaValue);
        ss << "instance: " << instance->cls->name;
    } else {
        DIE << "evaValueToConstantString: unknown type " << (int)evaValue.type;
    }

    return ss.str();
}

inline std::ostream& operator<<(std::ostream& os, const EvaValue& evaValue) {
    return os << "EvaValue(" << evaValueToTypeString(evaValue) << "): " << evaValueToConstantString(evaValue);
}

#endif