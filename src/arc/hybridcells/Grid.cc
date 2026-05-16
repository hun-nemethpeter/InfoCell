#include "arc/Color.h"
#include "arc/Grid.h"
#include "cells/Brain.h"

#include "Grid.h"

namespace nativearc = infocell::arc::native;

namespace infocell {
namespace cells {
namespace arc {

// ============================================================================
Grid::Grid(brain::Brain& kb, nativearc::Grid& picture) :
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

    for (const infocell::arc::Color& color : picture.pixels()) {
        int colorId = (int)color.id();
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
    visitor.visit(*this);
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

} // namespace arc
} // namespace cells
} // namespace infocell