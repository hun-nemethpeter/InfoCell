#include "cells/Brain.h"
#include "cells/Cells.h"
#include "DeprecatedCells.h"

#include <fmt/core.h>
#include <sstream>
#include <utility>

namespace infocell {
namespace cells {
namespace deprecated {

#pragma region Color
// ============================================================================
Color::Color(brain::Brain& kb, const infocell::arc::Color& inputColor) :
    CellI(kb),
    m_inputColor(inputColor)
{
}

bool Color::has(CellI& key)
{
    if (&key == &kb.ids.struct_) {
        return true;
    }
    if (&key == &kb.ids.red || &key == &kb.ids.green || &key == &kb.ids.blue) {
        return true;
    }

    return false;
}

void Color::set(CellI& key, CellI& value)
{
    throw "Changing a hybrid color cell is not possible!";
}

void Color::erase(CellI& key)
{
    throw "Changing a hybrid color cell is not possible!";
}

void Color::operator()()
{
    // Do nothing
}

CellI& Color::operator[](CellI& key)
{
    if (&key == &kb.ids.struct_) {
        return kb.std.Color;
    }
    if (&key == &kb.ids.red) {
        return kb.pools.numbers.get(m_inputColor.m_red);
    }
    if (&key == &kb.ids.green) {
        return kb.pools.numbers.get(m_inputColor.m_green);
    }
    if (&key == &kb.ids.blue) {
        return kb.pools.numbers.get(m_inputColor.m_blue);
    }

    throw "No such key!";
}

void Color::accept(Visitor& visitor)
{
//    visitor.visit(*this);
}

const infocell::arc::Color& Color::color() const
{
    return m_inputColor;
}
#pragma endregion
#pragma region Pixel
// ============================================================================
Pixel::Pixel(brain::Brain& kb, int x, int y, const infocell::arc::Color& inputColor) :
    CellI(kb),
    m_x(kb.pools.numbers.get(x)),
    m_y(kb.pools.numbers.get(y)),
    m_color(kb, inputColor),
    m_inputColor(inputColor)
{
}

bool Pixel::has(CellI& key)
{
    if (&key == &kb.ids.struct_) {
        return true;
    }
    if (&key == &kb.directions.up && m_up) {
        return true;
    }
    if (&key == &kb.directions.down && m_down) {
        return true;
    }
    if (&key == &kb.directions.left && m_left) {
        return true;
    }
    if (&key == &kb.directions.right && m_right) {
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
    if (&key == &kb.directions.up && m_up) {
        return *m_up;
    }
    if (&key == &kb.directions.down && m_down) {
        return *m_down;
    }
    if (&key == &kb.directions.left && m_left) {
        return *m_left;
    }
    if (&key == &kb.directions.right && m_right) {
        return *m_right;
    }
    if (&key == &kb.ids.color) {
        return m_color;
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

const infocell::arc::Color& Pixel::color() const
{
    return m_inputColor;
}
#pragma endregion
#pragma region Picture
// ============================================================================
Picture::Picture(brain::Brain& kb, infocell::arc::input::Grid& grid) :
    CellI(kb, grid.label()),
    m_width(grid.width()),
    m_height(grid.height()),
    m_widthCell(kb.pools.numbers.get(m_width)),
    m_heightCell(kb.pools.numbers.get(m_height))
{
    const int senzorSize = m_height * m_width;

    m_pixels.clear();
    m_pixels.reserve(senzorSize);

    int x = 0;
    int y = 0;

    for (const infocell::arc::Color& pixel : grid.pixels()) {
        m_pixels.emplace_back(kb, x++, y, pixel);
        if (x == m_width) {
            x = 0;
            y += 1;
        }
    }

    for (y = 0; y < m_height; ++y) {
        for (x = 0; x < m_width; ++x) {
            Pixel& pixel  = m_pixels[currentIndex(x, y)];
            pixel.m_up    = upPixel(x, y);
            pixel.m_down  = downPixel(x, y);
            pixel.m_left  = leftPixel(x, y);
            pixel.m_right = rightPixel(x, y);
        }
    }
    m_pixelsList.reset(new List(kb, m_pixels));
}

bool Picture::has(CellI& key)
{
    if (&key == &kb.ids.struct_ || &key == &kb.ids.width || &key == &kb.ids.height || &key == &kb.ids.pixels) {
        return true;
    }

    return false;
}

void Picture::set(CellI& key, CellI& value)
{
    throw "Changing a hybrid picture cell is not possible!";
}

void Picture::erase(CellI& key)
{
    throw "Changing a hybrid picture cell is not possible!";
}

void Picture::operator()()
{
}

CellI& Picture::operator[](CellI& key)
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

    throw "No such key!";
}

void Picture::accept(Visitor& visitor)
{
//    visitor.visit(*this);
}

Pixel& Picture::getPixel(int x, int y)
{
    return m_pixels[currentIndex(x, y)];
}

const Pixel& Picture::getPixel(int x, int y) const
{
    return m_pixels.at(currentIndex(x, y));
}

int Picture::currentIndex(int x, int y) const
{
    return y * m_width + x;
}

bool Picture::isInRange(int x, int y) const
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

 upPixel(4, 4) = 19
 3 * 5 + 4

 */

Pixel* Picture::upPixel(int x, int y)
{
    if (!isInRange(x, y) || y == 0) {
        return nullptr;
    }
    int upIndex = (y - 1) * m_width + x;
    return &m_pixels[upIndex];
}

Pixel* Picture::downPixel(int x, int y)
{
    if (!isInRange(x, y) || y == m_height - 1) {
        return nullptr;
    } else {
        int downIndex = (y + 1) * m_width + x;
        return &m_pixels[downIndex];
    }
}

Pixel* Picture::leftPixel(int x, int y)
{
    if (!isInRange(x, y) || x == 0) {
        return nullptr;
    } else {
        return &m_pixels[y * m_width + x - 1];
    }
}

Pixel* Picture::rightPixel(int x, int y)
{
    if (!isInRange(x, y) || x == m_width - 1) {
        return nullptr;
    } else {
        return &m_pixels[y * m_width + x + 1];
    }
}

std::vector<Pixel>& Picture::pixels()
{
    return m_pixels;
}

int Picture::width() const
{
    return m_width;
}

int Picture::height() const
{
    return m_height;
}
#pragma endregion

} // namespace hybrid
} // namespace cells
} // namespace infocell
