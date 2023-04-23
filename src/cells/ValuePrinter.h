#pragma once
#include <sstream>

#include "Brain.h"

namespace synth {
namespace cells {

class CellValuePrinter : public Visitor
{
public:
    void visit(Map::Index::Type::Slots::SlotList::Item& cell) override;
    void visit(Map::Index::Type::Slots::SlotList& cell) override;
    void visit(Map::Index::Type::Slots::SlotIndex& cell) override;
    void visit(Map::Index::Type::Slots& cell) override;
    void visit(Map::Index::Type::Slot& cell) override;
    void visit(Map::Index::Type& cell) override;
    void visit(Map::Index& cell) override;
    void visit(Map& cell) override;
    void visit(Object& cell) override;
    void visit(List::Item& cell) override;
    void visit(List& cell) override;
    void visit(Number& cell) override;
    void visit(String& cell) override;
    void visit(hybrid::Color& cell) override;
    void visit(hybrid::Pixel& cell) override;
    void visit(hybrid::Picture& cell) override;

    std::string print() const;

protected:
    void printImpl(CellI& list);
    void printOpBlock(CellI& cell);
    void printOpEvalVar(CellI& cell);
    void printOpFunction(CellI& cell);
    void printOpDelete(CellI& cell);
    void printOpSet(CellI& cell);
    void printOpIf(CellI& cell);
    void printOpDo(CellI& cell);
    void printOpWhile(CellI& cell);
    void printOpConstVar(CellI& cell);
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
    void printOpGreaterThan(CellI& cell);
    void printOpReturn(CellI& cell);
    void printAstCell(CellI& cell);
    void printAstGet(CellI& cell);
    void printAstInput(CellI& cell);
    void printAstOutput(CellI& cell);
    void printAstVar(CellI& cell);
    void printAstMember(CellI& cell);
    void printIndent();

    std::stringstream m_ss;
    mutable int m_indent = 0;
    const int m_tabSize = 4;
};

} // namespace cells
} // namespace synth