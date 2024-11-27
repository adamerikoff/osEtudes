#ifndef SCOPE_H
#define SCOPE_H

#include "../includes.h"
#include "../bytecode/OpCode.h"
#include "../logger/Logger.h"

enum class ScopeType {
    GLOBAL,
    FUNCTION,
    BLOCK,
    CLASS,
};

enum class AllocType {
    GLOBAL,
    LOCAL,
    CELL,
};

struct Scope {
    Scope(ScopeType type, std::shared_ptr<Scope> parent)
        : scopeType(type), parent(parent) {}

    void addLocal(const std::string& name);

    void addCell(const std::string& name);

    void addFree(const std::string& name);

    void maybePromote(const std::string& name);

    void promote(const std::string& name, Scope* ownerScope);

    uint8_t getNameGetter(const std::string& name) const;

    uint8_t getNameSetter(const std::string& name) const;

    std::pair<Scope*, AllocType> resolve(
        const std::string& name,
        AllocType allocType);
    ScopeType scopeType;
    std::shared_ptr<Scope> parent;
    std::map<std::string, AllocType> allocInfo;
    std::set<std::string> free;
    std::set<std::string> cells;
};
#endif