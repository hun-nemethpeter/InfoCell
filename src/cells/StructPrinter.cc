#include "StructPrinter.h"
#include "ValuePrinter.h"
#include "Brain.h"

namespace synth {
namespace cells {

void CellStructPrinter::visit(Map::Index::Type::Slots::SlotList::Item& cell)
{
    printImpl(cell);
}

void CellStructPrinter::visit(Map::Index::Type::Slots::SlotList& cell)
{
    printImpl(cell);
}

void CellStructPrinter::visit(Map::Index::Type::Slots::SlotIndex& cell)
{
    printImpl(cell);
}

void CellStructPrinter::visit(Map::Index::Type::Slots& cell)
{
    printImpl(cell);
}

void CellStructPrinter::visit(Map::Index::Type::Slot& cell)
{
    brain::Brain& kb = cell.kb;
    if (cell.label().empty()) {
        m_ss << cell[kb.coding.slotRole].label() << ": ";
    } else {
        m_ss << cell.label() << ": ";
    }
    printImpl(cell);
}

void CellStructPrinter::visit(Map::Index::Type& cell)
{
    printImpl(cell);
}

void CellStructPrinter::visit(Map::Index& cell)
{
    if (cell.label().empty()) {
        m_ss << "IndexedList::ValueIndex: ";
    } else {
        m_ss << cell.label() << ": ";
    }
    printImpl(cell);
}

void CellStructPrinter::visit(Map& cell)
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

void CellStructPrinter::visit(List::Item& cell)
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
    const bool needId = false;
    brain::Brain& kb = cell.kb;
    auto is           = [this, &cell, &kb](CellI& type) -> bool { return &cell.type() == &type || (cell.type().has(kb.coding.memberOf) && cell.type()[kb.coding.memberOf][kb.coding.index].has(type)); };

    CellI& type   = cell.type();
    if (&type == &kb.type.Type_) {
        if (!cell.label().empty()) {
            m_ss << cell.label() << ": ";
        }
    }
    if (&type == &kb.type.Slot) {
        if (cell.label().empty()) {
            m_ss << cell[kb.coding.slotRole].label() << ": ";
        } else {
            m_ss << cell.label() << ": ";
        }
    }

    if (is(kb.type.List)) {
        m_ss << "List<" << cell[kb.coding.objectType].label() << ">";
    } else if (is(kb.type.ListItem)) {
        m_ss << "ListItem<" << cell.type()[kb.coding.slots][kb.coding.index][kb.coding.value][kb.coding.slotType].label() << ">";
    } else if (is(kb.type.Map)) {
        m_ss << "Map<" << cell[kb.coding.keyType].label() << ", " << cell[kb.coding.objectType].label() << ">";
    } else {
        m_ss << "(" << type.label() << ")";
    }
    if (needId)
        m_ss << " ID" << &cell;
    m_ss << std::endl;
    CellValuePrinter typePrinter;
    type.accept(typePrinter);
    m_ss << "    +--(type)--> (" << type.label() << ")";
    if (needId)
        m_ss << " ID" << &type;
    m_ss << " // " << typePrinter.print() << std::endl;
    if (type.has(kb.coding.slots)) {
        CellI& slotList = type[kb.coding.slots][kb.coding.list];
        visitList(slotList, [this, &kb, &cell](CellI& slot, int i) {
            CellI& role = slot[kb.coding.slotRole];
            if (!cell.has(role)) {
                return;
            }
            CellValuePrinter valuePrinter;
            CellI& slotType      = slot[kb.coding.slotType];
            CellI& connectedCell = cell[role];
            connectedCell.accept(valuePrinter);
            m_ss << "    +--(" << role.label() << ")--> (" << slotType.label() << ")";
            if (needId)
                m_ss << " ID " << &connectedCell;
            m_ss << " // " << valuePrinter.print() << std::endl;
        });
    }
}

std::string CellStructPrinter::print() const
{
    return m_ss.str();
}

} // namespace cells
} // namespace synth