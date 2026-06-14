#pragma once

#include "Library.h"
#include "World.h"

namespace infocell {
namespace cells {

class StdLib : public Library
{
public:
    StdLib(World& w, Ast::Scope& parentScope);
};

} // namespace cells
} // namespace infocell