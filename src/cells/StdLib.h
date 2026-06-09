#pragma once

#include "Brain.h"

namespace infocell {
namespace cells {

class AstStd : public AstHelper
{
public:
    AstStd(Brain& kb);

private:
    void createOp();
    void createAst();

    Scope& stdScope;
};

} // namespace cells
} // namespace infocell