#include "StructPrinter.h"
#include "ValuePrinter.h"
#include "cells/Brain.h"

namespace infocell {
namespace cells {

void CellStructPrinter::visit(CellI& cell)
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

void CellStructPrinter::visit(Struct& cell)
{
    printImpl(cell);
}

void CellStructPrinter::visit(Index& cell)
{
    printImpl(cell);
}

void CellStructPrinter::visit(Map& cell)
{
    printImpl(cell);
}

void CellStructPrinter::visit(TrieMap& cell)
{
    printImpl(cell);
}

void CellStructPrinter::visit(Set& cell)
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

#if 0 // TODO
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
#endif

void CellStructPrinter::printImpl(CellI& cell)
{
    const bool needId = false;
    brain::Brain& kb = cell.kb;
    auto is           = [this, &cell, &kb](CellI& type) -> bool { return &cell.struct_() == &type || (cell.struct_().has(kb.ids.memberOf) && cell.struct_()[kb.ids.memberOf][kb.ids.index].has(type)); };

    CellI& type   = cell.struct_();
    if (&type == &kb.std.Struct) {
        if (!cell.label().empty()) {
            m_ss << cell.label() << ": ";
        }
    }
    if (&type == &kb.std.Slot) {
        if (cell.label().empty()) {
            m_ss << cell[kb.ids.key].label() << ": ";
        } else {
            m_ss << cell.label() << ": ";
        }
    }

    if (is(kb.std.List)) {
        m_ss << "List<" << cell.struct_()[kb.ids.subTypes][kb.ids.index][kb.ids.valueType][kb.ids.value].label() << ">";
    } else if (is(kb.std.ListItem)) {
        m_ss << "ListItem<" << cell.struct_()[kb.ids.subTypes][kb.ids.index][kb.ids.valueType][kb.ids.value].label() << ">";
    } else if (is(kb.std.Map)) {
        m_ss << "Map<" << cell.struct_()[kb.ids.subTypes][kb.ids.index][kb.ids.keyType][kb.ids.value].label() << ", " << cell.struct_()[kb.ids.subTypes][kb.ids.index][kb.ids.valueType][kb.ids.value].label() << ">";
    } else {
        m_ss << "(" << type.label() << ")";
    }
    if (needId)
        m_ss << " ID" << &cell;
    m_ss << std::endl;
    CellValuePrinter typePrinter;
    type.accept(typePrinter);
    m_ss << "    +-(struct)-> " << type.label();
    if (needId)
        m_ss << " ID" << &type;
    //     m_ss << " // " << typePrinter.print();
    m_ss << std::endl;
    if (type.has(kb.ids.slots)) {
        CellI& slotList = type[kb.ids.slots][kb.ids.list];
        visitList(slotList, [this, &kb, &cell, &needId](CellI& slot, int i, bool&) {
            CellI& role = slot[kb.ids.key];
            if (!cell.has(role)) {
                return;
            }
            CellValuePrinter valuePrinter;
            CellI& type          = slot[kb.ids.type];
            CellI& connectedCell = cell[role];
            connectedCell.accept(valuePrinter);
            m_ss << "    +-- " << role.label() << " --> " << type.label();
            if (needId)
                m_ss << " ID " << &connectedCell;
            // m_ss << " // " << valuePrinter.print();
            m_ss << std::endl;
        });
    }
}

std::string CellStructPrinter::print() const
{
    return m_ss.str();
}

} // namespace cells
} // namespace infocell