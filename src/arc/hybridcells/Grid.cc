#include <array>

#include "arc/Color.h"
#include "arc/Grid.h"
#include "cells/World.h"

#include "Grid.h"

namespace nativearc = infocell::arc::native;

namespace infocell {
namespace cells {
namespace arc {

// ============================================================================
Grid::Grid(World& w, nativearc::Grid& grid) :
    CellI(w, grid.label()),
    m_width(grid.width()),
    m_height(grid.height()),
    m_widthCell(w.pools.numbers.get(m_width)),
    m_heightCell(w.pools.numbers.get(m_height)),
    m_pixelsMap(w, w.std.Cell, w.arc.Pixel, "PixelsMap")
{
    static std::array<CellI*, 10> arcColorEnumValues = {
        &w.arc.Color.black,
        &w.arc.Color.blue,
        &w.arc.Color.red,
        &w.arc.Color.green,
        &w.arc.Color.yellow,
        &w.arc.Color.grey,
        &w.arc.Color.fuschia,
        &w.arc.Color.orange,
        &w.arc.Color.teal,
        &w.arc.Color.brown
    };

    const int gridSize = m_height * m_width;

    m_pixels.clear();
    m_pixels.reserve(gridSize);

    int x = 0;
    int y = 0;

    for (const infocell::arc::Color& color : grid.pixels()) {
        CellI& arcColorEnumValue = *arcColorEnumValues[(int)color.id()];
        m_pixels.emplace_back(w, x, y, arcColorEnumValue, *this);
        List pixelContent(w, w.arc.Pixel);
        pixelContent.add(w.pools.numbers.get(x));
        pixelContent.add(w.pools.numbers.get(y));
        m_pixelsMap.add(pixelContent, arcColorEnumValue);
        x = x + 1;
        if (x == m_width) {
            x = 0;
            y += 1;
        }
    }

    m_pixelsList.reset(new List(w, m_pixels));
}

bool Grid::has(CellI& key)
{
    if (&key == &w.id.__type__ || &key == &w.id.width || &key == &w.id.height || &key == &w.id.pixels || &key == &w.id.pixelsMap) {
        return true;
    }

    return false;
}

void Grid::set(CellI& key, CellI& value)
{
    throw "Changing a hybrid grid cell is not possible!";
}

void Grid::erase(CellI& key)
{
    throw "Changing a hybrid grid cell is not possible!";
}

void Grid::operator()()
{
}

CellI& Grid::operator[](CellI& key)
{
    if (&key == &w.id.__type__) {
        return w.arc.Grid;
    }
    if (&key == &w.id.width) {
        return m_widthCell;
    }
    if (&key == &w.id.height) {
        return m_heightCell;
    }
    if (&key == &w.id.pixels) {
        return *m_pixelsList;
    }
    if (&key == &w.id.pixelsMap) {
        return m_pixelsMap;
    }

    throw "No such key!";
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