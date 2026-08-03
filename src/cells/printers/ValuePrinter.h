#pragma once
#include <sstream>

#include "cells/NodeBase.h"
#include "cells/World.h"

namespace infocell {
namespace cells {

class CellValuePrinter : public NodeBase
{
public:
    CellValuePrinter(World& w);
    std::string print(CellI& cell);

protected:
    void printImpl(CellI& list);
    void printTypeName(CellI& cell);
    void printOpBlock(CellI& cell);
    void printOpActivate(CellI& cell);
    void printOpFunction(CellI& cell);
    void printOpCall(CellI& cell);
    void printOpDelete(CellI& cell);
    void printOpSet(CellI& cell);
    void printOpErase(CellI& cell);
    void printOpIf(CellI& cell);
    void printOpDo(CellI& cell);
    void printOpWhile(CellI& cell);
    void printOpConstVar(CellI& cell);
    void printOpUnknownVar(CellI& cell);
    void printOpVar(CellI& cell);
    void printOpNew(CellI& cell);
    void printOpSame(CellI& cell);
    void printOpNotSame(CellI& cell);
    void printOpEqual(CellI& cell);
    void printOpNotEqual(CellI& cell);
    void printOpHas(CellI& cell);
    void printOpMissing(CellI& cell);
    void printOpGet(CellI& cell);
    void printOpAnd(CellI& cell);
    void printOpOr(CellI& cell);
    void printOpNot(CellI& cell);
    void printOpAdd(CellI& cell);
    void printOpSubtract(CellI& cell);
    void printOpMultiply(CellI& cell);
    void printOpDivide(CellI& cell);
    void printOpLessThan(CellI& cell);
    void printOpLessThanOrEqual(CellI& cell);
    void printOpGreaterThan(CellI& cell);
    void printOpGreaterThanOrEqual(CellI& cell);
    void printOpReturn(CellI& cell);
    void printAstCell(CellI& cell);
    void printAstGet(CellI& cell);
    void printAstMember(CellI& cell);
    void printAstParameter(CellI& cell);
    void printAstReturn(CellI& cell);
    void printAstSet(CellI& cell);
    void printAstSubtract(CellI& cell);
    void printAstVar(CellI& cell);
    void printIndent();
    bool isThisCallAGetter(CellI& callAst);
    void prefixByAstVariableType(CellI& astVariable);

    std::stringstream m_ss;
    mutable int m_indent = 0;
    const int m_tabSize = 4;
};

} // namespace cells
} // namespace infocell