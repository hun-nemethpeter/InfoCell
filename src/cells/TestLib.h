#pragma once

#include "Library.h"
#include "World.h"

namespace infocell {
namespace cells {

class TestLib : public Library
{
public:
    TestLib(World& w, Ast::Scope& parentScope);
};

} // namespace cells
} // namespace infocell