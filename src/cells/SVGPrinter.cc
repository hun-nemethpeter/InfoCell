#include "SVGPrinter.h"

#include <format>
#include <iostream>

#include "fsvgui/dom/elements.h"

using namespace fsvgui;

namespace synth {
namespace cells {
namespace svg {

Printer::Printer(int width, int height) :
    m_screen(width, height)
{
    m_fontSize = 14;
    m_fontName = "Times New Roman";
    m_fontPath = "C:\\Windows\\Fonts\\times.ttf";
}

std::string Printer::print(Slot& cell)
{
    std::stringstream ss;
    return ss.str();
}

std::string Printer::print(Type& cell)
{
    std::stringstream ss;
    return ss.str();
}

std::string Printer::print(Object& cell)
{
    std::stringstream ss;
    return ss.str();
}

std::string Printer::print(ListItem& cell)
{
    std::stringstream ss;
    return ss.str();
}

std::string Printer::print(List& list)
{
    std::stringstream ss;

    Elements listItems;
    int i = 0;
    for (const ListItem& item : list.items()) {
        listItems.push_back(text(item.name())->fontSize(22)->fontColor({ 255, 20, 30 }) | center | borderWidth(10));
    }
    FlexboxConfig flexConfig;
    flexConfig.direction       = FlexboxConfig::Direction::Row;
    flexConfig.wrap            = FlexboxConfig::Wrap::NoWrap;
    flexConfig.justify_content = FlexboxConfig::JustifyContent::FlexStart;
    flexConfig.align_items     = FlexboxConfig::AlignItems::Center;
    flexConfig.align_content   = FlexboxConfig::AlignContent::FlexStart;

    auto document = flexbox(listItems, flexConfig);
    Render(m_screen, document);
    ss << m_screen.toString();

    return ss.str();
}

std::string Printer::print(Number& cell)
{
    std::stringstream ss;
    return ss.str();
}

std::string Printer::print(String& cell)
{
    std::stringstream ss;
    return ss.str();
}

std::string Printer::print(hybrid::Color& cell)
{
    std::stringstream ss;
    return ss.str();
}

std::string Printer::print(hybrid::Pixel& cell)
{
    std::stringstream ss;
    return ss.str();
}

std::string Printer::print(hybrid::Sensor& sensor)
{
    std::stringstream ss;
    int width = sensor.width();
    int height = sensor.width();
    int x      = 0;
    int y      = 0;
    const int pixelWidth = 40;
    const int pixelHeight = 40;
    const int spaceBetweenPixels = 1;
//    const int spaceBeforePixels  = height * 2 + 1;
    const int spaceBeforePixels  = 0;
    for (const hybrid::Pixel& pixel : sensor.pixels()) {
        int startDrawX = x * (pixelWidth + spaceBeforePixels + spaceBetweenPixels);
        int startDrawY = y * (pixelHeight + spaceBetweenPixels);
        int pixelDrawX = startDrawX + spaceBeforePixels;
        int pixelDrawY = startDrawY;

        int line1DrawX1 = startDrawX + (y * 2) + 1;
        int line1DrawY1 = startDrawY + pixelHeight / 2;
        int line1DrawX2 = line1DrawX1;
        int line1DrawY2 = line1DrawY1 + ((height - y) * (pixelHeight + spaceBetweenPixels));

        int line2DrawX1 = line1DrawX1;
        int line2DrawY1 = line1DrawY1;
        int line2DrawX2 = pixelDrawX;
        int line2DrawY2 = line1DrawY1;

        ss << std::format("<rect x=\"{}\" y=\"{}\" width=\"{}\" height=\"{}\" style=\"fill:rgb({}, {}, {})\" />\n", pixelDrawX, pixelDrawY, pixelWidth, pixelHeight, pixel.color().red(), pixel.color().green(), pixel.color().blue());
        //ss << std::format("<polyline points=\"{},{} {},{} {},{}\" style=\"fill:none;stroke:grey\" />\n", line2DrawX2, line2DrawY2, line1DrawX1, line1DrawY1, line1DrawX2, line1DrawY2);
        x += 1;
        if (x == width) {
            x = 0;
            y += 1;
        }
    }
    return ss.str();
}

void Printer::writeFile(const std::filesystem::path& path)
{
    m_screen.writeFile(path);
}

} // namespace svg
} // namespace cells
} // namespace synth
