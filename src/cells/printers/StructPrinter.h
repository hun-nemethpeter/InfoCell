#pragma once
#include <sstream>

#include "cells/NodeBase.h"
#include "cells/World.h"

namespace infocell {
namespace cells {

class CellStructPrinter : public NodeBase
{
public:
    CellStructPrinter(World& w);
    std::string print(CellI& cell);

protected:
    void printImpl(CellI& cell);

    std::stringstream m_ss;
};

} // namespace cells
} // namespace infocell