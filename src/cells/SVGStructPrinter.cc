#include "SVGStructPrinter.h"

#include <format>
#include <iostream>

#include "fsvgui/dom/elements.h"

using namespace fsvgui;

namespace synth {
namespace cells {
namespace svg {

StructPrinter::StructPrinter(int width, int height) :
    m_width(width), m_height(height)
{
    m_fontSize = 14;
    m_fontName = "Times New Roman";
}

void StructPrinter::visit(Map::Index::Type::Slots::SlotList::Item& cell)
{
    printStruct(cell);
}

void StructPrinter::visit(Map::Index::Type::Slots::SlotList& cell)
{
    printStruct(cell);
}

void StructPrinter::visit(Map::Index::Type::Slots::SlotIndex& cell)
{
    printStruct(cell);
}

void StructPrinter::visit(Map::Index::Type::Slots& cell)
{
    printStruct(cell);
}

void StructPrinter::visit(Map::Index::Type::Slot& cell)
{
    printStruct(cell);
}

void StructPrinter::visit(Map::Index::Type& cell)
{
    printStruct(cell);
}

void StructPrinter::visit(Map::Index& cell)
{
    printStruct(cell);
}

void StructPrinter::visit(Map& cell)
{
    printStruct(cell);
}

void StructPrinter::visit(Object& cell)
{
    printStruct(cell);
}

void StructPrinter::visit(List::Item& cell)
{
    printStruct(cell);
}

void StructPrinter::visit(List& cell)
{
    printStruct(cell);
}

void StructPrinter::visit(Number& cell)
{
    printStruct(cell);
}

void StructPrinter::visit(String& cell)
{
    printStruct(cell);
}

void StructPrinter::visit(hybrid::Color& cell)
{
    printStruct(cell);
}

void StructPrinter::visit(hybrid::Pixel& cell)
{
    printStruct(cell);
}

void StructPrinter::visit(hybrid::Picture& cell)
{
    printStruct(cell);
}

void StructPrinter::printStruct(CellI& cell)
{
    brain::Brain& kb = cell.kb;
    CellI& type      = cell.type();
    fsvgui::Color cellTypeColor(255, 0, 0);
    fsvgui::Color roleColor(163, 21, 21);
    fsvgui::Color typeColor(0, 0, 0) ;
    std::vector<Elements> lines;

    FlexboxConfig flexConfig;
    flexConfig.direction       = FlexboxConfig::Direction::Row;
    flexConfig.wrap            = FlexboxConfig::Wrap::NoWrap;
    flexConfig.justify_content = FlexboxConfig::JustifyContent::FlexEnd;
    flexConfig.align_items     = FlexboxConfig::AlignItems::FlexStart;
    flexConfig.align_content   = FlexboxConfig::AlignContent::FlexStart;

    std::string cellLabel;
    if (!cell.label().empty()) {
        cellLabel += "(";
        cellLabel += cell.label();
        cellLabel += ")";
    }
    cellLabel += " ";
    cellLabel += std::format("{}", (void*)&cell);
    Element svgTypeName   = text(cellLabel)->fontSize(16)->fontColor(cellTypeColor);
    lines.push_back({ flexbox({ text("type")->fontSize(14)->fontColor(roleColor) }, flexConfig),
                      filler() | size(WIDTH, EQUAL, 10),
                      text(type.label())->fontSize(14)->fontColor(typeColor),
                      filler() | size(WIDTH, EQUAL, 10),
                      text(std::format("{}", (void*)&cell[kb.coding.type]))->fontSize(14)->fontColor(typeColor) });

    if (type.has(kb.coding.slots)) {
        CellI& slotList = type[kb.coding.slots][kb.coding.list];
        visitList(slotList, [this, &kb, &cell, &roleColor, &typeColor, &lines, &flexConfig](CellI& slot, int i) {
            CellI& role = slot[kb.coding.slotRole];
            if (!cell.has(role)) {
                return;
            }

            CellI& slotType                = slot[kb.coding.slotType];
            CellI& connectedCell           = cell[role];
            std::string connectedCellLabel = connectedCell.label().empty() ? std::format("A {}", connectedCell.type().label()) : connectedCell.label();

            lines.push_back({ filler() | size(HEIGHT, EQUAL, 2), filler() | size(WIDTH, EQUAL, 10), filler() | size(HEIGHT, EQUAL, 2) });
            lines.push_back({ flexbox({ text(role.label())->fontSize(14)->fontColor(roleColor) }, flexConfig),
                              filler() | size(WIDTH, EQUAL, 10),
                              text(connectedCellLabel)->fontSize(14)->fontColor(typeColor),
                              filler() | size(WIDTH, EQUAL, 10),
                              text(std::format("{}", (void*)&cell[role]))->fontSize(14)->fontColor(typeColor) });
        });
    }
    m_stack.push(vbox(svgTypeName | center,
                      filler() | size(HEIGHT, EQUAL, 8),
                      gridbox(lines)));
}

void StructPrinter::showcaseLastResult(const std::string& caseName)
{
    m_showcaseItems.push_back(
        vbox(
            text(caseName) | center,
            filler() | size(HEIGHT, EQUAL, 4),
            m_stack.top() | center | borderWidth(10))
        | center);
}

std::string StructPrinter::print()
{
    Screen screen = processResult();
    Render(screen, m_stack.top());
    return screen.toString();
}

void StructPrinter::writeFile(const std::filesystem::path& path)
{
    if (m_stack.empty()) {
        return;
    }
    Screen screen = processResult();
    Render(screen, m_stack.top());
    screen.writeFile(path);
}

Screen StructPrinter::processResult()
{
    Screen screen({ 1920, 1080 });
    if (m_showcaseItems.empty()) {
        screen = Screen::Create(Dimension::Fit(m_stack.top()));
    }

    if (m_showcaseItems.empty()) {
        return screen;
    }
    FlexboxConfig flexButtonsConfig;
    flexButtonsConfig.direction       = FlexboxConfig::Direction::Row;
    flexButtonsConfig.wrap            = FlexboxConfig::Wrap::NoWrap;
    flexButtonsConfig.justify_content = FlexboxConfig::JustifyContent::SpaceEvenly;
    flexButtonsConfig.align_items     = FlexboxConfig::AlignItems::FlexStart;
    flexButtonsConfig.align_content   = FlexboxConfig::AlignContent::SpaceAround;

    auto result = flexbox(m_showcaseItems, flexButtonsConfig);
    m_stack.push(result);

    return screen;
}

} // namespace svg
} // namespace cells
} // namespace synth
