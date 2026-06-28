#include "StructPrinter.h"
#include "ValuePrinter.h"
#include "cells/World.h"

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

void CellStructPrinter::visit(List::Node& cell)
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
    World& w = cell.w;
    auto is           = [this, &cell, &w](CellI& type) -> bool { return &cell.__type__() == &type || (cell.__type__().has(w.id.memberOf) && cell.__type__()[w.id.memberOf][w.id.index].has(type)); };

    CellI& type   = cell.__type__();
    if (&type == &w.std.Struct) {
        if (!cell.label().empty()) {
            m_ss << cell.label() << ": ";
        }
    }
    if (&type == &w.std.Slot) {
        if (cell.label().empty()) {
            m_ss << cell[w.id.key].label() << ": ";
        } else {
            m_ss << cell.label() << ": ";
        }
    }

    if (is(w.std.List)) {
        m_ss << "List<" << cell.__type__()[w.id.typeAliases][w.id.index][w.id.valueType][w.id.value].label() << ">";
    } else if (is(w.std.ListNode)) {
        m_ss << "ListNode<" << cell.__type__()[w.id.typeAliases][w.id.index][w.id.valueType][w.id.value].label() << ">";
    } else if (is(w.std.Map)) {
        m_ss << "Map<" << cell.__type__()[w.id.typeAliases][w.id.index][w.id.keyType][w.id.value].label() << ", " << cell.__type__()[w.id.typeAliases][w.id.index][w.id.valueType][w.id.value].label() << ">";
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
    if (type.has(w.id.slots)) {
        CellI& slotList = type[w.id.slots][w.id.list];
        visitList(slotList, [this, &w, &cell, &needId](CellI& slot, int i, bool&) {
            CellI& role = slot[w.id.key];
            if (!cell.has(role)) {
                return;
            }
            CellValuePrinter valuePrinter;
            CellI& type          = slot[w.id.type];
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