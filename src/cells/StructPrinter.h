#pragma once
#include <sstream>

#include "Brain.h"

namespace synth {
namespace cells {

class CellStructPrinter : public Visitor
{
public:
    void visit(Object& cell) override;
    void visit(List::Item& cell) override;
    void visit(List& cell) override;
    void visit(Type& cell) override;
    void visit(Index& cell) override;
    void visit(Map& cell) override;
    void visit(Set& cell) override;
    void visit(Number& cell) override;
    void visit(String& cell) override;
    void visit(hybrid::Color& cell) override;
    void visit(hybrid::Pixel& cell) override;
    void visit(hybrid::Picture& cell) override;

    std::string print() const;

protected:
    void printImpl(CellI& cell);

    std::stringstream m_ss;
};

} // namespace cells
} // namespace synth