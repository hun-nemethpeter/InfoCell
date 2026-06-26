#pragma once

#include "cells/Library.h"
#include "cells/World.h"

namespace infocell {
namespace cells {

class TestSyms
{
public:
    TestSyms(World& w);

protected:
    World& w;

public:
    Object Color;
};

class TestLib : public Library
{
public:
    TestLib(World& w, Ast::Scope& parentScope, Compiler& compiler, TestSyms& test);
};

} // namespace cells
} // namespace infocell