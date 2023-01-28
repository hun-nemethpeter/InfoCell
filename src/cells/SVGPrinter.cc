#include "SVGPrinter.h"

#include <format>
#include <iostream>

#include "fsvgui/dom/elements.h"

using namespace fsvgui;

namespace synth {
namespace cells {
namespace svg {

Printer::Printer(int width, int height) :
    m_width(width), m_height(height)
{
    m_fontSize = 14;
    m_fontName = "Times New Roman";
}

void Printer::visit(Slot& cell)
{
    m_stack.push(text("Slot"));
}

void Printer::visit(Type& cell)
{
    m_stack.push(text("Type"));
}

void Printer::visit(Type_SlotMap& slotMap)
{
    m_stack.push(text("Type_SlotMap"));
}

void Printer::visit(Type_SlotMap_Type& slotMapType)
{
    m_stack.push(text("Type_SlotMap_Type"));
}

void Printer::visit(Type_SlotMap_Type_Slot& cell)
{
    m_stack.push(text("Type_SlotMap_Type_Slot"));
}

void Printer::visit(Type_SlotMap_Type_SlotList& cell)
{
    m_stack.push(text("Type_SlotMap_Type_SlotList"));
}

void Printer::visit(Type_SlotMap_Type_SlotList_Item& cell)
{
    m_stack.push(text("Type_SlotMap_Type_SlotList_Item"));
}

void Printer::visit(Type_SlotMap_Type_SlotMap& cell)
{
    m_stack.push(text("Type_SlotMap_Type_SlotMap"));
}



void Printer::visit(Object& cell)
{
    m_stack.push(text("Object"));
}

void Printer::visit(ListItem& cell)
{
    Element valueAsSvg;
    if (tryVisitWith(cell.value(), *this)) {
        valueAsSvg = m_stack.top();
        m_stack.pop();
    } else {
        valueAsSvg = text(cell.value().label())->fontSize(16)->fontColor({ 255, 0, 0 });
    }
    m_stack.push(hbox(vbox(text("ListItem") | center | borderWidth(10), filler() | size(HEIGHT, EQUAL, 2), valueAsSvg | center | borderWidth(10)),
                      filler() | size(WIDTH, EQUAL, 2)));
}

void Printer::visit(List& list)
{
    Elements listItems;
    visitList(list, [this, &listItems](CellI& value, int i) {
        Element valueAsSvg;
        if (tryVisitWith(value, *this)) {
            valueAsSvg = m_stack.top();
            m_stack.pop();
        } else {
            valueAsSvg = text(value.label())->fontSize(16)->fontColor({ 255, 0, 0 });
        }
        listItems.push_back(hbox(vbox(text(std::to_string(i + 1)) | center | borderWidth(10), filler() | size(HEIGHT, EQUAL, 2), valueAsSvg | center | borderWidth(10)),
                                 filler() | size(WIDTH, EQUAL, 2)));
    });

    FlexboxConfig flexConfig;
    flexConfig.direction       = FlexboxConfig::Direction::Row;
    flexConfig.wrap            = FlexboxConfig::Wrap::NoWrap;
    flexConfig.justify_content = FlexboxConfig::JustifyContent::FlexStart;
    flexConfig.align_items     = FlexboxConfig::AlignItems::Center;
    flexConfig.align_content   = FlexboxConfig::AlignContent::FlexStart;

    m_stack.push(flexbox(listItems, flexConfig));
}

void Printer::visit(Number& cell)
{
    m_stack.push(text(std::to_string(cell.value()))->fontColor({ 0, 0, 0 }));
}

void Printer::visit(String& cell)
{
    m_stack.push(text(cell.value())->fontColor({ 163, 21, 21 }));
}

void Printer::visit(hybrid::Color& cell)
{
    const int pixelWidth  = 20;
    const int pixelHeight = 20;
    m_stack.push(rect(pixelWidth, pixelHeight)->color({ cell.color().red(), cell.color().green(), cell.color().blue() }));
}

void Printer::visit(hybrid::Pixel& cell)
{
    const int pixelWidth  = 20;
    const int pixelHeight = 20;
    m_stack.push(rect(pixelWidth, pixelHeight)->color({ cell.color().red(), cell.color().green(), cell.color().blue() }));
}

void Printer::visit(hybrid::Picture& picture)
{
    int width  = picture.width();
    int height = picture.width();
    int x      = 0;
    int y      = 0;

    Elements columns;
    Elements row;
    for (hybrid::Pixel& pixel : picture.pixels()) {
        visit(pixel);
        if (!row.empty()) {
            row.push_back(filler() | size(WIDTH, EQUAL, 1));
        }
        row.push_back(m_stack.top());
        m_stack.pop();
        x += 1;
        if (x == width) {
            if (!columns.empty()) {
                columns.push_back(filler() | size(HEIGHT, EQUAL, 1));
            }
            columns.push_back(hbox(row));
            row = Elements();
            x = 0;
            y += 1;
        }
    }
    m_stack.push(vbox(columns) | center | borderWidth(20));
}

void Printer::showcaseLastResult(const std::string& caseName)
{
    m_showcaseItems.push_back(
        vbox(
            text(caseName) | center | borderWidth(10),
            filler() | size(HEIGHT, EQUAL, 2),
            m_stack.top() | center | borderWidth(10))
        | center | borderWidth(10));
}

std::string Printer::print()
{
    Screen screen = processResult();
    Render(screen, m_stack.top());
    return screen.toString();
}

void Printer::writeFile(const std::filesystem::path& path)
{
    if (m_stack.empty()) {
        return;
    }
    Screen screen = processResult();
    Render(screen, m_stack.top());
    screen.writeFile(path);
}

Screen Printer::processResult()
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
