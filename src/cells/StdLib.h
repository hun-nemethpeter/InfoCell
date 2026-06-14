#pragma once

#include "World.h"

namespace infocell {
namespace cells {

class StdLib : public AstHelper
{
public:
    StdLib(World& w, Scope& scope);

    Scope& stdScope;

private:
    void createOp();
    void createAst();
};

} // namespace cells
} // namespace infocell