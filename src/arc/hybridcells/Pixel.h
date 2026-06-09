#pragma once

#include "cells/Cells.h"
#include "cells/World.h"
#include "arc/Color.h"

namespace infocell {
namespace cells {
namespace arc {

// ============================================================================
class Grid;
class Pixel : public CellI
{
public:
    Pixel(World& w, int x, int y, CellI& arcColor, Grid& grid);

    bool has(CellI& key) override;
    void set(CellI& key, CellI& value) override;
    void erase(CellI& key) override;
    void operator()() override;
    CellI& operator[](CellI& key) override;
    void accept(Visitor& visitor) override;

    const int color() const;
    const infocell::arc::ColorId colorId() const;

    Number& m_x;
    Number& m_y;
    CellI& m_arcColor;

private:
    Grid& m_grid;
};

} // namespace arc
} // namespace cells
} // namespace infocell