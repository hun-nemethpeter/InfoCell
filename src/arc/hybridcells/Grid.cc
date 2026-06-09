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
    m_pixelsMap(w, w.std.Cell, w.std.Pixel, "PixelsMap")
{
    static CellI& ArcColorBlack   = w.getVariable("arc::Color::black");
    static CellI& ArcColorBlue    = w.getVariable("arc::Color::blue");
    static CellI& ArcColorRed     = w.getVariable("arc::Color::red");
    static CellI& ArcColorGreen   = w.getVariable("arc::Color::green");
    static CellI& ArcColorYellow  = w.getVariable("arc::Color::yellow");
    static CellI& ArcColorGrey    = w.getVariable("arc::Color::grey");
    static CellI& ArcColorFuschia = w.getVariable("arc::Color::fuschia");
    static CellI& ArcColorOrange  = w.getVariable("arc::Color::orange");
    static CellI& ArcColorTeal    = w.getVariable("arc::Color::teal");
    static CellI& ArcColorBrown   = w.getVariable("arc::Color::brown");
    static std::array<CellI*, 10> arcColorEnumValues = {
        &ArcColorBlack,
        &ArcColorBlue,
        &ArcColorRed,
        &ArcColorGreen,
        &ArcColorYellow,
        &ArcColorGrey,
        &ArcColorFuschia,
        &ArcColorOrange,
        &ArcColorTeal,
        &ArcColorBrown
    };

    const int gridSize = m_height * m_width;

    m_pixels.clear();
    m_pixels.reserve(gridSize);

    int x = 0;
    int y = 0;

    for (const infocell::arc::Color& color : grid.pixels()) {
        CellI& arcColorEnumValue = *arcColorEnumValues[(int)color.id()];
        m_pixels.emplace_back(w, x, y, arcColorEnumValue, *this);
        List pixelContent(w, w.std.Pixel);
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
    if (&key == &w.ids.struct_ || &key == &w.ids.width || &key == &w.ids.height || &key == &w.ids.pixels || &key == &w.ids.pixelsMap) {
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
    if (&key == &w.ids.struct_) {
        return w.std.Grid;
    }
    if (&key == &w.ids.width) {
        return m_widthCell;
    }
    if (&key == &w.ids.height) {
        return m_heightCell;
    }
    if (&key == &w.ids.pixels) {
        return *m_pixelsList;
    }
    if (&key == &w.ids.pixelsMap) {
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