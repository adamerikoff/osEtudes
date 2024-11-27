#ifndef EVACOLLECTOR_H
#define EVACOLLECTOR_H

#include "../includes.h"
#include "EvaValue.h"

struct EvaCollector {
    void gc(const std::set<Traceable *> &roots);
    void mark(const std::set<Traceable *> &roots);
    void sweep();
    std::set<Traceable *> getPointers(const Traceable *object);
};

#endif