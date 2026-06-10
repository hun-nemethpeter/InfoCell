#pragma once

#include "World.h"

namespace infocell {
namespace cells {

class StdLib : public AstHelper
{
public:
    StdLib(World& w);

private:
    void createOp();
    void createAst();

    Scope& stdScope;
};

} // namespace cells
} // namespace infocell