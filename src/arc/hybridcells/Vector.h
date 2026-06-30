#pragma once

#include "cells/Cells.h"

namespace infocell {
namespace cells {
namespace arc {

class Vector : public cells::CellI
{
public:
    Vector(World& w, Number& x, Number& y);

    bool has(CellI& key) override;
    void set(CellI& key, CellI& value) override;
    void erase(CellI& key) override;
    void operator()() override;
    CellI& operator[](CellI& key) override;

    std::string toString() const;

    Number& m_x;
    Number& m_y;
};
std::ostream& operator<<(std::ostream& os, const Vector& vector);

} // namespace arc
} // namespace cells
} // namespace infocell