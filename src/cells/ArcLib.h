#pragma once

#include "Library.h"
#include "World.h"

namespace infocell {
namespace cells {

class ArcLib : public Library
{
public:
    ArcLib(World& w, Ast::Scope& parentScope);
};

} // namespace cells
} // namespace infocell