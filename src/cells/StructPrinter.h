#pragma once
#include <sstream>

#include "Brain.h"

namespace synth {
namespace cells {

class CellStructPrinter : public Visitor
{
public:
    void visit(Type& cell) override;
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
    void visit(op::Function&) override;


    std::string print() const;

protected:
    void printImpl(CellI& cell);

    std::stringstream m_ss;
};

} // namespace cells
} // namespace synth