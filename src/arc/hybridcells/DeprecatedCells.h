#pragma once

#include <memory>
#include <vector>

#include "arc/Color.h"
#include "arc/Grid.h"
#include "cells/Cells.h"

namespace infocell {
namespace cells {
namespace deprecated {

// ============================================================================
class Color : public CellI
{
public:
    Color(brain::Brain& kb, const infocell::arc::Color& inputColor);

    bool has(CellI& key) override;
    void set(CellI& key, CellI& value) override;
    void erase(CellI& key) override;
    void operator()() override;
    CellI& operator[](CellI& key) override;
    void accept(Visitor& visitor) override;

    const infocell::arc::Color& color() const;

private:
    const infocell::arc::Color& m_inputColor;
};

// ============================================================================
class Pixel : public CellI
{
public:
    Pixel(brain::Brain& kb, int x, int y, const infocell::arc::Color& inputColor);

    bool has(CellI& key) override;
    void set(CellI& key, CellI& value) override;
    void erase(CellI& key) override;
    void operator()() override;
    CellI& operator[](CellI& key) override;
    void accept(Visitor& visitor) override;

    const infocell::arc::Color& color() const;

    Pixel* m_up    = nullptr;
    Pixel* m_down  = nullptr;
    Pixel* m_left  = nullptr;
    Pixel* m_right = nullptr;
    Number& m_x;
    Number& m_y;

private:
    Color m_color;
    const infocell::arc::Color& m_inputColor;
};

// ============================================================================
class Picture : public CellI
{
public:
    Picture(brain::Brain& kb, infocell::arc::native::Grid& grid);

    bool has(CellI& key) override;
    void set(CellI& key, CellI& value) override;
    void erase(CellI& key) override;
    void operator()() override;
    CellI& operator[](CellI& key) override;
    void accept(Visitor& visitor) override;

    Pixel& getPixel(int x, int y);
    const Pixel& getPixel(int x, int y) const;

    int currentIndex(int x, int y) const;
    bool isInRange(int x, int y) const;
    Pixel* upPixel(int x, int y);
    Pixel* downPixel(int x, int y);
    Pixel* leftPixel(int x, int y);
    Pixel* rightPixel(int x, int y);
    std::vector<Pixel>& pixels();

    int width() const;
    int height() const;

protected:
    int m_width;
    int m_height;
    Number& m_widthCell;
    Number& m_heightCell;
    std::vector<Pixel> m_pixels;
    std::unique_ptr<List> m_pixelsList;
};

} // namespace deprecated
} // namespace cells
} // namespace infocell