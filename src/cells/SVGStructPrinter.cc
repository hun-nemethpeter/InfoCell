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

void StructPrinter::visit(Slot& cell)
{
    printStruct(cell);
}

void StructPrinter::visit(Type& cell)
{
    printStruct(cell);
}

void StructPrinter::visit(Type_SlotMap& cell)
{
    printStruct(cell);
}

void StructPrinter::visit(Type_SlotMap_Type& cell)
{
    printStruct(cell);
}

void StructPrinter::visit(Type_SlotMap_Type_Slot& cell)
{
    printStruct(cell);
}

void StructPrinter::visit(Type_SlotMap_Type_SlotList& cell)
{
    printStruct(cell);
}

void StructPrinter::visit(Type_SlotMap_Type_SlotList_Item& cell)
{
    printStruct(cell);
}

void StructPrinter::visit(Type_SlotMap_Type_SlotMap& cell)
{
    printStruct(cell);
}

void StructPrinter::visit(Object& cell)
{
    printStruct(cell);
}

void StructPrinter::visit(ListItem& cell)
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
    fsvgui::Color typeColor(0, 0, 0);
    std::vector<Elements> lines;

    Element svgTypeName = text(cell.label())->fontSize(16)->fontColor(cellTypeColor);
    lines.push_back({ text("type")->fontSize(14)->fontColor(roleColor), filler() | size(WIDTH, EQUAL, 10), text(type.label())->fontSize(14)->fontColor(typeColor) });

    CellI& slotList = type[kb.cells.slotList];
    visitList(slotList, [this, &kb, &cell, &roleColor, &typeColor, &lines](CellI& slot, int i) {
        CellI& role = slot[kb.cells.slotRole];
        if (!cell.has(role)) {
            return;
        }

        CellI& slotType = slot[kb.cells.slotType];

        lines.push_back({ filler() | size(HEIGHT, EQUAL, 7), filler() | size(WIDTH, EQUAL, 10), filler() | size(HEIGHT, EQUAL, 7) });
        lines.push_back({ text(role.label())->fontSize(14)->fontColor(roleColor), filler() | size(WIDTH, EQUAL, 10), text(slotType.label())->fontSize(14)->fontColor(typeColor) });
    });
    m_stack.push(vbox(svgTypeName | center,
                      filler() | size(HEIGHT, EQUAL, 8),
                      gridbox(lines)));
}

void StructPrinter::showcaseLastResult(const std::string& caseName)
{
    m_showcaseItems.push_back(
        vbox(
            text(caseName) | center,
            filler() | size(HEIGHT, EQUAL, 2),
            m_stack.top() | center | borderWidth(10))
        | center | borderWidth(10));
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
