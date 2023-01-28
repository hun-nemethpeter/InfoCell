#include "ValuePrinter.h"

namespace synth {
namespace cells {

// ============================================================================
void CellValuePrinter::visit(Slot& cell)
{
    brain::Brain& kb = cell.kb;
    m_ss << cell[kb.cells.slotRole].label() << ": " << cell[kb.cells.slotType].label();
}

void CellValuePrinter::visit(Type& type)
{
    m_ss << "Type " << type.label() << " { ";
    bool isFirst = true;
    for (auto& slotI : type.slots()) {
        if (isFirst) {
            isFirst = false;
        } else {
            m_ss << ", ";
        }
        m_ss << slotI.first->label() << ": " << slotI.second.slotType().label();
    }
    m_ss << " }";
}

void CellValuePrinter::visit(Type_SlotMap& slotMap)
{
    printImpl(slotMap);
}

void CellValuePrinter::visit(Type_SlotMap_Type& slotMapType)
{
    printImpl(slotMapType);
}

void CellValuePrinter::visit(Type_SlotMap_Type_Slot& cell)
{
    brain::Brain& kb = cell.kb;
    m_ss << cell[kb.cells.slotRole].label() << ": " << cell[kb.cells.slotType].label();
}

void CellValuePrinter::visit(Type_SlotMap_Type_SlotList& cell)
{
    printImpl(cell);
}

void CellValuePrinter::visit(Type_SlotMap_Type_SlotList_Item& cell)
{
    printImpl(cell);
}

void CellValuePrinter::visit(Type_SlotMap_Type_SlotMap& cell)
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

    if (!cell.label().empty()) {
        m_ss << cell.label() << ": ";
    }
    m_ss << cell.type().label() << " { ";
    bool isFirst = true;

    CellI& cellType           = cell.type();
    CellI& slotList           = cellType[kb.cells.slotList];
    CellI* currentListItemPtr = slotList.has(kb.sequence.first) ? &slotList[kb.sequence.first] : nullptr;
    while (currentListItemPtr) {
        CellI& currentListItem = *currentListItemPtr;
        CellI& slot            = currentListItem[kb.coding.value];

        if (isFirst) {
            isFirst = false;
        } else {
            m_ss << ", ";
        }
        m_ss << ".";
        slot.accept(*this);

        currentListItemPtr = currentListItem.has(kb.sequence.next) ? &currentListItem[kb.sequence.next] : nullptr;
    }
    m_ss << " }";
}

void CellValuePrinter::visit(ListItem& listItemCell)
{
    m_ss << "[ ";
    if (!listItemCell.value().label().empty()) {
        m_ss << listItemCell.value().label() << " ";
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
        m_ss << " ";
        item.value().accept(*this);
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

std::string CellValuePrinter::print() const
{
    return m_ss.str();
}

} // namespace cells
} // namespace synth