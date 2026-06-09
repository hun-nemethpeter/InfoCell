#pragma once

#include "cells/Cells.h"
#include "cells/Brain.h"

namespace infocell {
namespace cells {

class NodeBase
{
protected:
    NodeBase(Brain& kb);

    CellI& getVariable(const std::string& name);
    CellI& getStruct(const std::string& name);
    CellI& getStruct(CellI& id);
    CellI& toCellNumber(int number);

    Brain& kb;
    ID& ids = kb.ids;
    CellI& _0_     = kb._0_;
    CellI& _1_     = kb._1_;
    CellI& _2_     = kb._2_;
    CellI& _3_     = kb._3_;
    CellI& _4_     = kb._4_;
    CellI& _5_     = kb._5_;
    CellI& _6_     = kb._6_;
    CellI& _7_     = kb._7_;
    CellI& _8_     = kb._8_;
    CellI& _9_     = kb._9_;
    CellI& true_   = kb.boolean.true_;
    CellI& false_  = kb.boolean.false_;
};

} // namespace cells
} // namespace infocell