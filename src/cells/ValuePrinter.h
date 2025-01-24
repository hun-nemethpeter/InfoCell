#pragma once
#include <sstream>

#include "Brain.h"

namespace infocell {
namespace cells {

class CellValuePrinter : public Visitor
{
public:

    void visit(Object& cell) override;
    void visit(List::Item& cell) override;
    void visit(List& cell) override;
    void visit(Struct& cell) override;
    void visit(Index& cell) override;
    void visit(Map& cell) override;
    void visit(TrieMap& cell) override;
    void visit(Set& cell) override;
    void visit(Number& cell) override;
    void visit(String& cell) override;
    void visit(hybrid::Color& cell) override;
    void visit(hybrid::Pixel& cell) override;
    void visit(hybrid::Picture& cell) override;

    std::string print() const;

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
    void printAstParameter(CellI& cell);
    void printAstVar(CellI& cell);
    void printAstMember(CellI& cell);
    void printAstSubtract(CellI& cell);
    void printIndent();

    std::stringstream m_ss;
    mutable int m_indent = 0;
    const int m_tabSize = 4;
};

} // namespace cells
} // namespace infocell