#pragma once
#include <sstream>

#include "Cells.h"

namespace synth {
namespace cells {

class CellStructPrinter : public Visitor
{
public:
    void visit(Slot& cell) override;
    void visit(Type& cell) override;
    void visit(Object& cell) override;
    void visit(ListItem& cell) override;
    void visit(List& cell) override;
    void visit(Number& cell) override;
    void visit(String& cell) override;
    void visit(hybrid::Color& cell) override;
    void visit(hybrid::Pixel& cell) override;
    void visit(hybrid::Sensor& cell) override;

    std::string print() const;

protected:
    void printImpl(CellI& cell);

    std::stringstream m_ss;
};

} // namespace cells
} // namespace synth