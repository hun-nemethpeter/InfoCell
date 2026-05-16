#include "Cells.h"
#include "cells/Brain.h"
#include "app/Colors.h"

#include <fmt/core.h>
#include <sstream>
#include <utility>

#pragma warning(disable : 4996)
#include <utf8.h>


namespace infocell {
namespace arc2026 {

#pragma region Pixel
// ============================================================================
Pixel::Pixel(brain::Brain& kb, int x, int y, int arcColor, Grid& grid) :
    CellI(kb),
    m_x(kb.pools.numbers.get(x)),
    m_y(kb.pools.numbers.get(y)),
    m_arcColor(kb.pools.numbers.get(arcColor)),
    m_grid(grid)
{
}

bool Pixel::has(CellI& key)
{
    if (&key == &kb.ids.struct_) {
        return true;
    }
    if (&key == &kb.directions.up && m_grid.hasPixel(m_x.value(), m_y.value() - 1)) {
        return true;
    }
    if (&key == &kb.directions.down && m_grid.hasPixel(m_x.value(), m_y.value() + 1)) {
        return true;
    }
    if (&key == &kb.directions.left && m_grid.hasPixel(m_x.value() - 1, m_y.value())) {
        return true;
    }
    if (&key == &kb.directions.right && m_grid.hasPixel(m_x.value() + 1, m_y.value())) {
        return true;
    }
    if (&key == &kb.ids.color) {
        return true;
    }
    if (&key == &kb.coordinates.x) {
        return true;
    }
    if (&key == &kb.coordinates.y) {
        return true;
    }

    return false;
}

void Pixel::set(CellI& key, CellI& value)
{
    throw "Changing a hybrid pixel cell is not possible!";
}

void Pixel::erase(CellI& key)
{
    throw "Changing a hybrid pixel cell is not possible!";
}

void Pixel::operator()()
{
    // Do nothing
}

CellI& Pixel::operator[](CellI& key)
{
    if (&key == &kb.ids.struct_) {
        return kb.std.Pixel;
    }
    if (&key == &kb.directions.up && m_grid.hasPixel(m_x.value(), m_y.value() - 1)) {
        return m_grid.getPixel(m_x.value(), m_y.value() - 1);
    }
    if (&key == &kb.directions.down && m_grid.hasPixel(m_x.value(), m_y.value() + 1)) {
        return m_grid.getPixel(m_x.value(), m_y.value() + 1);
    }
    if (&key == &kb.directions.left && m_grid.hasPixel(m_x.value() - 1, m_y.value())) {
        return m_grid.getPixel(m_x.value() - 1, m_y.value());
    }
    if (&key == &kb.directions.right && m_grid.hasPixel(m_x.value() + 1, m_y.value())) {
        return m_grid.getPixel(m_x.value() + 1, m_y.value());
    }
    if (&key == &kb.ids.color) {
        return m_arcColor;
    }
    if (&key == &kb.coordinates.x) {
        return m_x;
    }
    if (&key == &kb.coordinates.y) {
        return m_y;
    }

    throw "No such key!";
}

void Pixel::accept(Visitor& visitor)
{
//    visitor.visit(*this);
}

const int Pixel::color() const
{
    return m_arcColor.value();
}
#pragma endregion
#pragma region Picture
int getArcColorId(const input::Color& color)
{
    static input::Color black(0x00, 0x00, 0x00);
    static input::Color blue(0x00, 0x74, 0xD9);
    static input::Color red(0xFF, 0x41, 0x36);
    static input::Color green(0x2E, 0xCC, 0x40);
    static input::Color yellow(0xFF, 0xDC, 0x00);
    static input::Color grey(0xAA, 0xAA, 0xAA);
    static input::Color fuschia(0xF0, 0x12, 0xBE);
    static input::Color orange(0xFF, 0x85, 0x1B);
    static input::Color teal(0x7F, 0xDB, 0xFF);
    static input::Color brown(0x87, 0x0C, 0x25);

    static std::map<input::Color, infocell::arc::Colors> arcColorNames = {
        { black, infocell::arc::Colors::black },
        { blue, infocell::arc::Colors::blue },
        { red, infocell::arc::Colors::red },
        { green, infocell::arc::Colors::green },
        { yellow, infocell::arc::Colors::yellow },
        { grey, infocell::arc::Colors::grey },
        { fuschia, infocell::arc::Colors::fuschia },
        { orange, infocell::arc::Colors::orange },
        { teal, infocell::arc::Colors::teal },
        { brown, infocell::arc::Colors::brown }
    };

    return static_cast<int>(arcColorNames.find({ color.red(), color.green(), color.blue() })->second);
}
// ============================================================================
Grid::Grid(brain::Brain& kb, input::Grid& picture) :
    CellI(kb, picture.label()),
    m_width(picture.width()),
    m_height(picture.height()),
    m_widthCell(kb.pools.numbers.get(m_width)),
    m_heightCell(kb.pools.numbers.get(m_height)),
    m_pixelsMap(kb, kb.std.Cell, kb.std.Pixel, "PixelsMap")
{
    const int gridSize = m_height * m_width;

    m_pixels.clear();
    m_pixels.reserve(gridSize);

    int x = 0;
    int y = 0;

    for (const input::Color& color : picture.pixels()) {
        int colorId = getArcColorId(color);
        m_pixels.emplace_back(kb, x, y, colorId, *this);
        List pixelContent(kb, kb.std.Pixel);
        pixelContent.add(kb.pools.numbers.get(x));
        pixelContent.add(kb.pools.numbers.get(y));
        m_pixelsMap.add(pixelContent, kb.pools.numbers.get(colorId));
        x = x + 1;
        if (x == m_width) {
            x = 0;
            y += 1;
        }
    }

    m_pixelsList.reset(new List(kb, m_pixels));
}

bool Grid::has(CellI& key)
{
    if (&key == &kb.ids.struct_ || &key == &kb.ids.width || &key == &kb.ids.height || &key == &kb.ids.pixels || &key == &kb.ids.pixelsMap) {
        return true;
    }

    return false;
}

void Grid::set(CellI& key, CellI& value)
{
    throw "Changing a hybrid picture cell is not possible!";
}

void Grid::erase(CellI& key)
{
    throw "Changing a hybrid picture cell is not possible!";
}

void Grid::operator()()
{
}

CellI& Grid::operator[](CellI& key)
{
    if (&key == &kb.ids.struct_) {
        return kb.std.Grid;
    }
    if (&key == &kb.ids.width) {
        return m_widthCell;
    }
    if (&key == &kb.ids.height) {
        return m_heightCell;
    }
    if (&key == &kb.ids.pixels) {
        return *m_pixelsList;
    }
    if (&key == &kb.ids.pixelsMap) {
        return m_pixelsMap;
    }

    throw "No such key!";
}

void Grid::accept(Visitor& visitor)
{
//    visitor.visit(*this);
}

Pixel& Grid::getPixel(int x, int y)
{
    return m_pixels[currentIndex(x, y)];
}

const Pixel& Grid::getPixel(int x, int y) const
{
    return m_pixels.at(currentIndex(x, y));
}

int Grid::currentIndex(int x, int y) const
{
    return y * m_width + x;
}

bool Grid::hasPixel(int x, int y) const
{
    if (y < 0 || x < 0 || x > m_width - 1 || y > m_height - 1) {
        return false;
    }

    return true;
}

/*
       5 x 5
m_width  = 5
m_height = 5

     0  1  2  3  4
 0  00 01 02 03 04
 1  05 06 07 08 09
 2  10 11 12 13 14
 3  15 16 17 18 19
 4  20 21 22 23 24

 x=0, y=4 is 20

 upPixel(4, 4) = 19
 3 * 5 + 4

 */

std::vector<Pixel>& Grid::pixels()
{
    return m_pixels;
}

int Grid::width() const
{
    return m_width;
}

int Grid::height() const
{
    return m_height;
}

Number& Grid::widthCell()
{
    return m_widthCell;
}

Number& Grid::heightCell()
{
    return m_heightCell;
}

#pragma endregion

} // namespace arc2026
} // namespace infocell
