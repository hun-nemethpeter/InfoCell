#pragma once
#include <sstream>

#include "cells/World.h"

namespace infocell {
namespace cells {

class CellStructPrinter : public Visitor
{
public:
    void visit(CellI& cell) override;
    void visit(Object& cell) override;
    void visit(List::Node& cell) override;
    void visit(List& cell) override;
    void visit(Struct& cell) override;
    void visit(Index& cell) override;
    void visit(Map& cell) override;
    void visit(TrieMap& cell) override;
    void visit(Set& cell) override;
    void visit(Number& cell) override;
    void visit(String& cell) override;

    std::string print() const;

protected:
    void printImpl(CellI& cell);

    std::stringstream m_ss;
};

} // namespace cells
} // namespace infocell