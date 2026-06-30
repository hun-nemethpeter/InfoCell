#pragma once

#include "cells/World.h"

#include "Shape.h"
#include "Vector.h"

namespace infocell {
namespace cells {
namespace arc {

class ShapeField : public cells::CellI
{
public:
    ShapeField(World& w, int width, int height);

    using CellI::erase;
    using CellI::get;
    using CellI::has;
    using CellI::missing;
    using CellI::set;
    using CellI::operator[];

    bool has(CellI& key) override;
    void set(CellI& key, CellI& value) override;
    void erase(CellI& key) override;
    void operator()() override;
    CellI& operator[](CellI& key) override;

    void addShape(Vector& offset, Shape& shape);
    bool hasShape(Vector& offset);
    Shape& getShape(Vector& offset);

    Number m_width;
    Number m_height;
    cells::TrieMap m_shapesMap;
};

} // namespace arc
} // namespace cells
} // namespace infocell