#pragma once
#include <sstream>

#include "Cells.h"

namespace synth {
namespace cells {

class CellValuePrinter : public Visitor
{
public:
    void visit(Slot& cell) override;
    void visit(Type& cell) override;
    void visit(Type_SlotMap& cell) override;
    void visit(Type_SlotMap_Type& cell) override;
    void visit(Type_SlotMap_Type_Slot& cell) override;
    void visit(Type_SlotMap_Type_SlotList& cell) override;
    void visit(Type_SlotMap_Type_SlotList_Item& cell) override;
    void visit(Type_SlotMap_Type_SlotMap& cell) override;
    void visit(Object& cell) override;
    void visit(ListItem& cell) override;
    void visit(List& cell) override;
    void visit(Number& cell) override;
    void visit(String& cell) override;
    void visit(hybrid::Color& cell) override;
    void visit(hybrid::Pixel& cell) override;
    void visit(hybrid::Picture& cell) override;

    std::string print() const;

protected:
    void printImpl(CellI& list);

    std::stringstream m_ss;
};

} // namespace cells
} // namespace synth