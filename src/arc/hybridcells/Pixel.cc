#include "Pixel.h"
#include "Grid.h"

#include "arc/Color.h"
#include "cells/Brain.h"
#include "cells/Cells.h"

namespace infocell {
namespace cells {
namespace arc {

// ============================================================================
Pixel::Pixel(Brain& kb, int x, int y, CellI& arcColor, Grid& grid) :
    CellI(kb),
    m_x(kb.pools.numbers.get(x)),
    m_y(kb.pools.numbers.get(y)),
    m_arcColor(arcColor),
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
    visitor.visit(*this);
}

const int Pixel::color() const
{
    CellI& tagValue = m_arcColor["tag"];
    CellI& value    = m_arcColor[tagValue];
    return static_cast<Number&>(value).value();
}

const infocell::arc::ColorId Pixel::colorId() const
{
    return (infocell::arc::ColorId)color();
}

} // namespace arc
} // namespace cells
} // namespace infocell
