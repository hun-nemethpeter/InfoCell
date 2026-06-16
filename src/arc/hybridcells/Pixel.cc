#include "Pixel.h"
#include "Grid.h"

#include "arc/Color.h"
#include "cells/World.h"

namespace infocell {
namespace cells {
namespace arc {

// ============================================================================
Pixel::Pixel(World& w, int x, int y, CellI& arcColor, Grid& grid) :
    CellI(w),
    m_x(w.pools.numbers.get(x)),
    m_y(w.pools.numbers.get(y)),
    m_arcColor(arcColor),
    m_grid(grid)
{
}

bool Pixel::has(CellI& key)
{
    if (&key == &w.id.__type__) {
        return true;
    }
    if (&key == &w.directions.up && m_grid.hasPixel(m_x.value(), m_y.value() - 1)) {
        return true;
    }
    if (&key == &w.directions.down && m_grid.hasPixel(m_x.value(), m_y.value() + 1)) {
        return true;
    }
    if (&key == &w.directions.left && m_grid.hasPixel(m_x.value() - 1, m_y.value())) {
        return true;
    }
    if (&key == &w.directions.right && m_grid.hasPixel(m_x.value() + 1, m_y.value())) {
        return true;
    }
    if (&key == &w.id.color) {
        return true;
    }
    if (&key == &w.coordinates.x) {
        return true;
    }
    if (&key == &w.coordinates.y) {
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
    if (&key == &w.id.__type__) {
        return w.std.Pixel;
    }
    if (&key == &w.directions.up && m_grid.hasPixel(m_x.value(), m_y.value() - 1)) {
        return m_grid.getPixel(m_x.value(), m_y.value() - 1);
    }
    if (&key == &w.directions.down && m_grid.hasPixel(m_x.value(), m_y.value() + 1)) {
        return m_grid.getPixel(m_x.value(), m_y.value() + 1);
    }
    if (&key == &w.directions.left && m_grid.hasPixel(m_x.value() - 1, m_y.value())) {
        return m_grid.getPixel(m_x.value() - 1, m_y.value());
    }
    if (&key == &w.directions.right && m_grid.hasPixel(m_x.value() + 1, m_y.value())) {
        return m_grid.getPixel(m_x.value() + 1, m_y.value());
    }
    if (&key == &w.id.color) {
        return m_arcColor;
    }
    if (&key == &w.coordinates.x) {
        return m_x;
    }
    if (&key == &w.coordinates.y) {
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
