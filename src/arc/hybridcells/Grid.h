#pragma once

#include "cells/Cells.h"
#include "arc/Grid.h"
#include "Pixel.h"

namespace infocell {
namespace cells {
namespace arc {

// ============================================================================
class Grid : public CellI
{
public:
    Grid(World& w, infocell::arc::native::Grid& grid);

    bool has(CellI& key) override;
    void set(CellI& key, CellI& value) override;
    void erase(CellI& key) override;
    void operator()() override;
    CellI& operator[](CellI& key) override;
    void accept(Visitor& visitor) override;

    Pixel& getPixel(int x, int y);
    const Pixel& getPixel(int x, int y) const;
    bool hasPixel(int x, int y) const;

    int currentIndex(int x, int y) const;

    std::vector<Pixel>& pixels();

    int width() const;
    int height() const;
    Number& widthCell();
    Number& heightCell();

protected:
    int m_width;
    int m_height;
    Number& m_widthCell;
    Number& m_heightCell;
    std::vector<Pixel> m_pixels;
    std::unique_ptr<List> m_pixelsList;
    TrieMap m_pixelsMap;
};

} // namespace arc
} // namespace cells
} // namespace infocell