#pragma once

#include "cells/Cells.h"

namespace infocell {
namespace arc2026 {

// ============================================================================
class Grid;
class Pixel : public CellI
{
public:
    Pixel(brain::Brain& kb, int x, int y, int arcColor, Grid& grid);

    bool has(CellI& key) override;
    void set(CellI& key, CellI& value) override;
    void erase(CellI& key) override;
    void operator()() override;
    CellI& operator[](CellI& key) override;
    void accept(Visitor& visitor) override;

    const int color() const;

    Number& m_x;
    Number& m_y;
    Number& m_arcColor;

private:
    Grid& m_grid;
};

// ============================================================================
class Grid : public CellI
{
public:
    Grid(brain::Brain& kb, input::Grid& screen);

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
    Pixel* upPixel(int x, int y);
    Pixel* downPixel(int x, int y);
    Pixel* leftPixel(int x, int y);
    Pixel* rightPixel(int x, int y);
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

} // namespace arc2026
} // namespace infocell