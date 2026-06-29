#include "StructPrinter.h"
#include "ValuePrinter.h"
#include "cells/World.h"

namespace infocell {
namespace cells {

CellStructPrinter::CellStructPrinter(World& w) :
    NodeBase(w)
{
}

void CellStructPrinter::printImpl(CellI& cell)
{
    const bool needId = false;

    if (!cell.label().empty()) {
        m_ss << cell.label() << ": ";
    }

    CellI& type = cell.__type__();
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

    if (cell.isA(w.std.List)) {
        m_ss << "List<" << cell.__type__()[w.id.typeAliases][w.id.index][w.id.valueType][w.id.value].label() << ">";
    } else if (cell.isA(w.std.ListNode)) {
        m_ss << "ListNode<" << cell.__type__()[w.id.typeAliases][w.id.index][w.id.valueType][w.id.value].label() << ">";
    } else if (cell.isA(w.std.Map)) {
        m_ss << "Map<" << cell.__type__()[w.id.typeAliases][w.id.index][w.id.keyType][w.id.value].label() << ", " << cell.__type__()[w.id.typeAliases][w.id.index][w.id.valueType][w.id.value].label() << ">";
    } else {
        m_ss << "(" << type.label() << ")";
    }
    if (needId)
        m_ss << " ID" << &cell;
    m_ss << std::endl;
    m_ss << "    +-(struct)-> " << type.label();
    if (needId)
        m_ss << " ID" << &type;
    m_ss << std::endl;
    if (type.has(w.id.slots)) {
        CellI& slotList = cell.slotList();
        Visitor::visitList(slotList, [this, &cell, &needId](CellI& slot, int i, bool&) {
            CellI& role = slot[w.id.key];
            if (!cell.has(role)) {
                return;
            }
            CellI& type          = slot[w.id.type];
            CellI& connectedCell = cell[role];
            m_ss << "    +-- " << role.label() << " --> " << type.label();
            if (needId)
                m_ss << " ID " << &connectedCell;
            m_ss << std::endl;
        });
    }
}

std::string CellStructPrinter::print(CellI& cell)
{
    printImpl(cell);
    return m_ss.str();
}

} // namespace cells
} // namespace infocell