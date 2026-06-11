#pragma once

#include "World.h"

namespace infocell {
namespace cells {

class TestLib : public AstHelper
{
public:
    TestLib(World& w, Scope& scope);

    Scope& testScope;
};

} // namespace cells
} // namespace infocell