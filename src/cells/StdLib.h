#pragma once

#include "World.h"

namespace infocell {
namespace cells {

class AstStd : public AstHelper
{
public:
    AstStd(World& w);

private:
    void createOp();
    void createAst();

    Scope& stdScope;
};

} // namespace cells
} // namespace infocell