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
    m_fontPath = "C:\\Windows\\Fonts\\times.ttf";
}

void Printer::visit(Slot& cell)
{
}

void Printer::visit(Type& cell)
{
}

void Printer::visit(Object& cell)
{
}

void Printer::visit(ListItem& cell)
{
}

void Printer::visit(List& list)
{
    Elements listItems;
    int i = 0;
    for (ListItem& item : list.items()) {
        visit(item);
        auto svgItem = m_stack.top();
        m_stack.pop();
        listItems.push_back(text(item.name())->fontSize(22)->fontColor({ 255, 20, 30 }) | center | borderWidth(10));
    }
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
}

void Printer::visit(String& cell)
{
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

void Printer::visit(hybrid::Sensor& sensor)
{
    int width = sensor.width();
    int height = sensor.width();
    int x      = 0;
    int y      = 0;

    Elements columns;
    Elements row;
    for (hybrid::Pixel& pixel : sensor.pixels()) {
        visit(pixel);
        row.push_back(m_stack.top());
        m_stack.pop();
        x += 1;
        if (x == width) {
            columns.push_back(hbox(row));
            row = Elements();
            x = 0;
            y += 1;
        }
    }
    m_stack.push(vbox(columns) | center | borderWidth(20));
}

std::string Printer::print()
{
    auto screen = Screen::Create(Dimension::Fit(m_stack.top()));
    Render(screen, m_stack.top());
    return screen.toString();
}

void Printer::writeFile(const std::filesystem::path& path)
{
    auto screen  = Screen::Create(Dimension::Fit(m_stack.top()));
    Render(screen, m_stack.top());
    screen.writeFile(path);
}

} // namespace svg
} // namespace cells
} // namespace synth
