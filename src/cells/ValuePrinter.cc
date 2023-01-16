#include "ValuePrinter.h"

namespace synth {
namespace cells {

// ============================================================================
void CellValuePrinter::visit(Slot& slotCell)
{
    m_ss << slotCell.name() << ": " << slotCell.slotType().name();
}

void CellValuePrinter::visit(Type& type)
{
    m_ss << "class " << type.name() << " { ";
    bool isFirst = true;
    for (auto& slotI : type.slots()) {
        if (isFirst) {
            isFirst = false;
        } else {
            m_ss << ", ";
        }
        m_ss << slotI.first << ": " << slotI.second->slotType().name();
    }
    m_ss << " }";
}

void CellValuePrinter::visit(Object& dataCell)
{
    if (!dataCell.name().empty()) {
        m_ss << dataCell.name() << ": ";
    }
    m_ss << dataCell.type().name() << " { ";
    bool isFirst = true;
    for (auto& slotI : dataCell.type().slots()) {
        if (isFirst) {
            isFirst = false;
        } else {
            m_ss << ", ";
        }
        m_ss << ".";
        slotI.second->accept(*this);
    }
    m_ss << " }";
}

void CellValuePrinter::visit(ListItem& listItemCell)
{
    m_ss << "[ ";
    if (!listItemCell.value().name().empty()) {
        m_ss << listItemCell.value().name() << " ";
    }
    m_ss << "]";

}

void CellValuePrinter::visit(List& List)
{
    m_ss << "[";
    bool isFirst = true;
    for (auto& item : List.items()) {
        if (isFirst) {
            isFirst = false;
        } else {
            m_ss << ",";
        }
        m_ss << " " << item.value().name();
    }
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
    m_ss << "(Color) rgb(" << (int)cell.color().m_red << ", " << (int)cell.color().m_green << "" << (int)cell.color().m_blue << ")";
}

void CellValuePrinter::visit(hybrid::Pixel& cell)
{
    m_ss << "(Pixel) [" << (int)cell.color().m_red << ", " << (int)cell.color().m_green << "" << (int)cell.color().m_blue << "]";
}

void CellValuePrinter::visit(hybrid::Sensor& cell)
{
    m_ss << "(Sensor)" << cell.name() << "[" << cell.width() << ", " << cell.height() << "]";
}

std::string CellValuePrinter::print() const
{
    return m_ss.str();
}

} // namespace cells
} // namespace synth