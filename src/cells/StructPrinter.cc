#include "StructPrinter.h"
#include "ValuePrinter.h"

namespace synth {
namespace cells {

// ============================================================================
void CellStructPrinter::visit(Slot& cell)
{
    brain::Brain& kb = cell.kb;
    if (cell.label().empty()) {
        m_ss << cell[kb.cells.slotRole].label() << ": ";
    } else {
        m_ss << cell.label() << ": ";
    }
    printImpl(cell);
}
void CellStructPrinter::visit(Type& cell)
{
    if (!cell.label().empty()) {
        m_ss << cell.label() << ": ";
    }
    printImpl(cell);
}

void CellStructPrinter::visit(Type_SlotMap& cell)
{
    if (cell.label().empty()) {
        m_ss << "SlotMap: ";
    } else {
        m_ss << cell.label() << ": ";
    }
    printImpl(cell);
}

void CellStructPrinter::visit(Type_SlotMap_Type& cell)
{
    printImpl(cell);
}

void CellStructPrinter::visit(Type_SlotMap_Type_Slot& cell)
{
    brain::Brain& kb = cell.kb;
    if (cell.label().empty()) {
        m_ss << cell[kb.cells.slotRole].label() << ": ";
    } else {
        m_ss << cell.label() << ": ";
    }
    printImpl(cell);
}

void CellStructPrinter::visit(Type_SlotMap_Type_SlotList& cell)
{
    printImpl(cell);
}

void CellStructPrinter::visit(Type_SlotMap_Type_SlotList_Item& cell)
{
    printImpl(cell);
}

void CellStructPrinter::visit(Type_SlotMap_Type_SlotMap& cell)
{
    printImpl(cell);
}

void CellStructPrinter::visit(Object& cell)
{
    if (!cell.label().empty()) {
        m_ss << cell.label() << ": ";
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

void CellStructPrinter::visit(hybrid::Picture& cell)
{
    printImpl(cell);
}

void CellStructPrinter::printImpl(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    CellI& type   = cell.type();
    m_ss << "(" << type.label() << ") ID" << &cell << std::endl;
    CellValuePrinter typePrinter;
    type.accept(typePrinter);
    m_ss << "    +--(type)--> (" << type.label() << ") ID" << &type << " // " << typePrinter.print() << std::endl;
    CellI& slotList = type[kb.cells.slotList];

    for (CellI* currentListItemPtr = slotList.has(kb.sequence.first) ? &slotList[kb.sequence.first] : nullptr; currentListItemPtr; currentListItemPtr = (*currentListItemPtr).has(kb.sequence.next) ? &(*currentListItemPtr)[kb.sequence.next] : nullptr) {
        CellI& currentListItem = *currentListItemPtr;
        CellI& slot            = currentListItem[kb.coding.value];
        CellI& role            = slot[kb.cells.slotRole];

        if (!cell.has(role)) {
            continue;
        }
        CellValuePrinter valuePrinter;
        CellI& slotType      = slot[kb.cells.slotType];
        CellI& connectedCell = cell[role];
        connectedCell.accept(valuePrinter);
        m_ss << "    +--(" << role.label() << ")--> (" << slotType.label() << ") ID" << &connectedCell << " // " << valuePrinter.print() << std::endl;
    }
}

std::string CellStructPrinter::print() const
{
    return m_ss.str();
}

} // namespace cells
} // namespace synth