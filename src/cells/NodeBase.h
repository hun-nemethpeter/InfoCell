#pragma once

#include "cells/Cells.h"
#include "cells/World.h"

namespace infocell {
namespace cells {

class NodeBase
{
protected:
    NodeBase(World& w);

    CellI& getVariable(const std::string& name);
    CellI& getStruct(const std::string& name);
    CellI& getStruct(CellI& id);
    CellI& toCellNumber(int number);

    World& w;

    ID& id   = w.id;
    Ast& ast = w.ast;
    Std& std = w.std;
    Op& op   = w.op;
    Arc& arc = w.arc;

    CellI& _0_    = w._0_;
    CellI& _1_    = w._1_;
    CellI& _2_    = w._2_;
    CellI& _3_    = w._3_;
    CellI& _4_    = w._4_;
    CellI& _5_    = w._5_;
    CellI& _6_    = w._6_;
    CellI& _7_    = w._7_;
    CellI& _8_    = w._8_;
    CellI& _9_    = w._9_;

    CellI& true_  = w.std.Boolean.true_;
    CellI& false_ = w.std.Boolean.false_;
};

} // namespace cells
} // namespace infocell