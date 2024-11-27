#ifndef GLOBAL_H
#define GLOBAL_H

#include "../includes.h"
#include "EvaValue.h"

struct GlobalVar {
    std::string Name;
    EvaValue value;
};

struct Global {
    std::vector<GlobalVar> globals;

    void set(size_t index, const EvaValue& value);
    void define(const std::string& name);
    int getGlobalIndex(const std::string& name);
    void addConst(const std::string& name, double value);
    bool exists(const std::string& name);
    GlobalVar get(const size_t index);
    void addNativeFunction(const std::string& name, std::function<void()> fn, size_t arity);
};

#endif