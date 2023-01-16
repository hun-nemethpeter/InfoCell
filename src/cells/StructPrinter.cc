#include "StructPrinter.h"
#include "ValuePrinter.h"

namespace synth {
namespace cells {

// ============================================================================
void CellStructPrinter::visit(Slot& cell)
{
    if (!cell.name().empty()) {
        m_ss << cell.name() << ": ";
    }
    printImpl(cell);
}

void CellStructPrinter::visit(Type& cell)
{
    if (!cell.name().empty()) {
        m_ss << cell.name() << ": ";
    }
    printImpl(cell);
}

void CellStructPrinter::visit(Object& cell)
{
    if (!cell.name().empty()) {
        m_ss << cell.name() << ": ";
    }
    printImpl(cell);
}

void CellStructPrinter::visit(ListItem& cell)
{
    printImpl(cell);
}

void CellStructPrinter::visit(List& cell)
{
    printImpl(cell);
}

void CellStructPrinter::visit(Number& cell)
{
    printImpl(cell);
}

void CellStructPrinter::visit(String& cell)
{
    printImpl(cell);
}

void CellStructPrinter::visit(hybrid::Color& cell)
{
    printImpl(cell);
}

void CellStructPrinter::visit(hybrid::Pixel& cell)
{
    printImpl(cell);
}

void CellStructPrinter::visit(hybrid::Sensor& cell)
{
    printImpl(cell);
}

void CellStructPrinter::printImpl(CellI& cell)
{
    Type& type = cell.type();
    m_ss << "(" << type.name() << ") ID" << &cell << std::endl;
    for (auto& slotI : type.slots()) {
        const std::string& slotSlotName = slotI.first;
        Slot& slot                      = *slotI.second;

        if (!cell.has(slot.slotRole())) {
            continue;
        }
        CellValuePrinter valuePrinter;
        Type& slotType       = static_cast<Type&>(slot[Slot::slotSlotType()]);
        CellI& connectedCell = cell[slot.slotRole()];
        connectedCell.accept(valuePrinter);
        m_ss << "    +--(" << slotSlotName << ")--> (" << slotType.name() << ") ID" << &connectedCell << " // " << valuePrinter.print() << std::endl;
    }
}

std::string CellStructPrinter::print() const
{
    return m_ss.str();
}

} // namespace cells
} // namespace synth