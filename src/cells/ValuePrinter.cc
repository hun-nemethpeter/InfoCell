#include "ValuePrinter.h"
#include "Brain.h"

namespace synth {
namespace cells {

void CellValuePrinter::visit(Type& type)
{
    brain::Brain& kb = type.kb;
    m_ss << "Type " << type.label();
    visitList(type[kb.cells.memberOf][kb.cells.list], [this, &kb](CellI& member, int i) {
        if (i != 0) {
            m_ss << ", ";
        } else {
            m_ss << " : ";
        }
        m_ss << member.label();
    });
    m_ss << " { ";
    visitList(type[kb.cells.slots][kb.cells.list], [this, &kb](CellI& slot, int i) {
        if (i != 0) {
            m_ss << ", ";
        }
        m_ss << slot[kb.cells.slotRole].label() << ": " << slot[kb.cells.slotType].label();
    });
    m_ss << " }";
}

void CellValuePrinter::visit(Map::Index::Type::Slots::SlotList::Item& cell)
{
    printImpl(cell);
}

void CellValuePrinter::visit(Map::Index::Type::Slots::SlotList& cell)
{
    printImpl(cell);
}

void CellValuePrinter::visit(Map::Index::Type::Slots::SlotIndex& cell)
{
    printImpl(cell);
}

void CellValuePrinter::visit(Map::Index::Type::Slots& cell)
{
    printImpl(cell);
}

void CellValuePrinter::visit(Map::Index::Type::Slot& cell)
{
    brain::Brain& kb = cell.kb;
    m_ss << cell[kb.cells.slotRole].label() << ": " << cell[kb.cells.slotType].label();
}

void CellValuePrinter::visit(Map::Index::Type& cell)
{
    printImpl(cell);
}

void CellValuePrinter::visit(Map::Index& cell)
{
    printImpl(cell);
}

void CellValuePrinter::visit(Map& cell)
{
    printImpl(cell);
}

void CellValuePrinter::visit(Object& object)
{
    printImpl(object);
}

void CellValuePrinter::printImpl(CellI& cell)
{
    brain::Brain& kb = cell.kb;

    if (&cell.type() == &kb.type.Slot) {
        m_ss << cell[kb.cells.slotRole].label() << ": " << cell[kb.cells.slotType].label();
        return;
    }

    if (cell.type()[kb.cells.memberOf][kb.cells.index].has(kb.type.List) || &cell.type() == &kb.type.List) {
        m_ss << "[";
        visitList(cell, [this](CellI& value, int i) {
            if (i != 0) {
                m_ss << ",";
            }
            m_ss << " ";
            value.accept(*this);
        });
        m_ss << " ]";
        return;
    }

    if (!cell.label().empty()) {
        m_ss << cell.label() << ": ";
    }
    m_ss << cell.type().label() << " { ";

    visitList(cell.type()[kb.cells.slots][kb.cells.list], [this, &kb](CellI& slot, int i) {
        if (i != 0) {
            m_ss << ", ";
        }
        m_ss << ".";
        slot.accept(*this);
    });

    m_ss << " }";
}

void CellValuePrinter::visit(List::Item& listItemCell)
{
    brain::Brain& kb = listItemCell.kb;
    m_ss << "[ ";
    if (!listItemCell[kb.coding.value].label().empty()) {
        m_ss << listItemCell[kb.coding.value].label() << " ";
    }
    m_ss << "]";

}

void CellValuePrinter::visit(List& list)
{
    m_ss << "[";
    visitList(list, [this](CellI& value, int i) {
        if (i != 0) {
            m_ss << ",";
        }
        m_ss << " ";
        value.accept(*this);
    });
    m_ss << " ]";
}

void CellValuePrinter::visit(Number& cell)
{
    m_ss << "(Number) " << cell.value();
}

void CellValuePrinter::visit(String& cell)
{
    m_ss << "(String) \"" << cell.value() << "\"";
}

void CellValuePrinter::visit(hybrid::Color& cell)
{
    m_ss << "Color(" << (int)cell.color().m_red << "," << (int)cell.color().m_green << "," << (int)cell.color().m_blue << ")";
}

void CellValuePrinter::visit(hybrid::Pixel& cell)
{
    m_ss << "Pixel[" << (int)cell.color().m_red << "," << (int)cell.color().m_green << "," << (int)cell.color().m_blue << "]";
}

void CellValuePrinter::visit(hybrid::Picture& cell)
{
    m_ss << "(Picture)" << cell.label() << "[" << cell.width() << ", " << cell.height() << "]";
}

void CellValuePrinter::visit(op::Function& cell)
{
    printImpl(cell);
}

std::string CellValuePrinter::print() const
{
    return m_ss.str();
}

} // namespace cells
} // namespace synth